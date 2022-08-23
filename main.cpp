#include "asio.hpp"
#include "livelink.hpp"
#include "mediapipe.hpp"
#include <iostream>
#include <thread>
#include <vector>

class Logger : public MediapipeLogger {
public:
    virtual void Log(const std::string& content) const override {
        std::cout << content << std::endl;
    }
};

int main() {
    // init asio udp socket
    asio::io_context ioContext;
    asio::ip::udp::resolver resolver(ioContext);
    asio::ip::udp::endpoint serverEndpoint(asio::ip::address::from_string("127.0.0.1"), 11111);
    asio::ip::udp::socket socket(ioContext);
    socket.open(asio::ip::udp::v4());

    // init mediapipe and set logger
    auto logger = std::make_shared<Logger>();
    auto interface = CreateMediapipeInterface();
    interface->SetLogger(logger);

    LiveLink::FaceLiveLink faceLiveLink;

    constexpr char windowName[] = "MediaPipe";
    cv::namedWindow(windowName);
    cv::VideoCapture capture;

    capture.open("D:/video/pml.mp4");
    bool isCamera = false;

    cv::Mat outputBGRFrame;
    bool grabFrames = true;
    if (!capture.isOpened()) {
        logger->Log("VideoCapture is not open");
    }
    interface->SetResourceDir("");
    interface->SetGraph("holistic_tracking_onnx_cuda.pbtxt");

    auto matCallback = [&](const cv::Mat& frame) {
        cv::cvtColor(frame, outputBGRFrame, cv::COLOR_RGB2BGR);
        cv::imshow(windowName, outputBGRFrame);
    };
    interface->OpenPreview(matCallback);

    auto sendCallback = [&](const std::vector<char> buffer) {
        socket.send_to(asio::buffer(buffer), serverEndpoint);
    };

    auto landmarkCallback = [&](const std::vector<std::vector<double>>& data) {
        faceLiveLink.Send(sendCallback);
    };

    interface->CreateObserver("face_landmarks", landmarkCallback);
    interface->Start();

    while (grabFrames) {
        cv::Mat cameraBGRFrame;
        capture >> cameraBGRFrame;
        if (isCamera) {
            cv::flip(cameraBGRFrame, cameraBGRFrame, 1);
        }
        if (cameraBGRFrame.empty()) {
            std::cout << "Empty frame." << std::endl;
            break;
        }
        cv::Mat cameraRGBFrame;
        cv::cvtColor(cameraBGRFrame, cameraRGBFrame, cv::COLOR_BGR2RGB);
        interface->Detect(cameraRGBFrame);
        const int pressed_key = cv::waitKey(40);
        if (pressed_key >= 0 && pressed_key != 255) grabFrames = false;
    }

    interface->Stop();
    delete interface;
    return 0;
}