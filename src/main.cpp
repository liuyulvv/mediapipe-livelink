#include "asio.hpp"
#include "mediapipe_library.h"
#include "util.hpp"
#include <opencv2/opencv.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

std::array<int, 51> resort_index{
    9,
    11,
    13,
    15,
    17,
    19,
    21,
    10,
    12,
    14,
    16,
    18,
    20,
    22,
    23,
    24,
    26,
    25,
    27,
    32,
    38,
    33,
    39,
    44,
    45,
    30,
    31,
    28,
    29,
    46,
    47,
    40,
    41,
    42,
    43,
    36,
    37,
    34,
    35,
    48,
    49,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    50,
    51,
};

int main() {
    // init asio udp socket
    asio::io_context ioContext;
    asio::ip::udp::resolver resolver(ioContext);
    asio::ip::udp::endpoint serverEndpoint(asio::ip::address::from_string("127.0.0.1"), 11111);
    asio::ip::udp::socket sock(ioContext);

    sock.open(asio::ip::udp::v4());
    size_t blend_shape_size = 52;
    float* blend_shape_list = new float[blend_shape_size];
    const std::string GRAPH_PATH = "mediapipe/graphs/face_blendshape/face_blendshape_desktop_live.pbtxt";
    CreateFaceBlendShapeInterface(GRAPH_PATH.c_str());

    cv::Mat camera_bgr_frame;
    cv::VideoCapture capture;
    capture.open(0);
    bool is_camera = true;
    bool grab_frame = true;
    if (!capture.isOpened()) {
        delete[] blend_shape_list;
        return -1;
    }

    std::vector<char> buffer_;
    const std::string uuid_ = "$fcaf7061-2964-459b-87a9-ad78290e21e6";
    const std::string name_ = "liuyulvv";

    AddFaceBlendShapePoller();

    StartFaceBlendShape();

    cv::namedWindow("camera");

    while (grab_frame) {
        capture >> camera_bgr_frame;
        if (is_camera) {
            cv::flip(camera_bgr_frame, camera_bgr_frame, 1);
        }
        if (camera_bgr_frame.empty()) {
            break;
        }
        cv::Mat camera_rgb_frame;
        cv::cvtColor(camera_bgr_frame, camera_rgb_frame, cv::COLOR_BGR2RGB);
        FaceBlendShapeProcess(&camera_rgb_frame);
        GetFaceBlendShapeOutput(blend_shape_list, blend_shape_size);
        if (blend_shape_size && blend_shape_list) {
            for (int i = 0; i < blend_shape_size; ++i) {
                buffer_.clear();
                Util::ValueToBuffer(6, buffer_, false);  // version
                for (const auto& c : uuid_) {            // uuid
                    buffer_.push_back(c);
                }
                Util::ValueToBuffer(int(name_.size()), buffer_, true);  // name length
                for (const auto& c : name_) {                           // name
                    buffer_.push_back(c);
                }
                Util::ValueToBuffer(0, buffer_, true);   // ???
                Util::ValueToBuffer(0, buffer_, true);   // ???
                Util::ValueToBuffer(60, buffer_, true);  // fps
                Util::ValueToBuffer(1, buffer_, true);   // int(fps/60)
                buffer_.push_back(0x3d);                 // blendshape length
                for (const auto& index : resort_index) {
                    Util::ValueToBuffer(blend_shape_list[index], buffer_, true);
                }
                for (int i = 0; i < 10; ++i) {
                    Util::ValueToBuffer(0.0F, buffer_, true);
                }
                sock.send_to(asio::buffer(buffer_), serverEndpoint);
            }
        }
        cv::imshow("camera", camera_bgr_frame);
        int pressed_key = cv::waitKey(30);
        if (pressed_key >= 0 && pressed_key != 255) grab_frame = false;
    }
    delete[] blend_shape_list;
    cv::destroyAllWindows();
    StopFaceBlendShape();
    ReleaseFaceBlendShapeInterface();
    return 0;
}