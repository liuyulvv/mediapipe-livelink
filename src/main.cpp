#include "asio.hpp"
#include "livelink.hpp"
#include "mediapipe.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

using json = nlohmann::json;

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

    // init blend shape config
    std::ifstream config("blend_shape_config.json");
    auto jsonData = json::parse(config);

    LiveLink::FaceLiveLink faceLiveLink(jsonData);

    constexpr char windowName[] = "MediaPipe";
    cv::namedWindow(windowName);
    cv::VideoCapture capture;

    capture.open(0);
    bool isCamera = true;

    cv::Mat outputBGRFrame;
    cv::Mat cameraBGRFrame;
    bool grabFrames = true;
    if (!capture.isOpened()) {
        logger->Log("VideoCapture is not open");
    }
    interface->SetResourceDir("");
    interface->SetGraph("iris_tracking_cpu.pbtxt");

    auto matCallback = [&](const cv::Mat& frame) {
        cv::cvtColor(frame, outputBGRFrame, cv::COLOR_RGB2BGR);
        cv::imshow(windowName, outputBGRFrame);
    };
    interface->OpenPreview(matCallback);

    auto sendCallback = [&](const std::vector<char> buffer) {
        socket.send_to(asio::buffer(buffer), serverEndpoint);
    };

    faceLiveLink.Renew(sendCallback);

    auto landmarkCallback = [&](const std::vector<std::vector<double>>& data) {
        faceLiveLink.Process(sendCallback, data);
    };

    auto tempCallback = [&](const std::vector<std::vector<double>>& data) {
        // faceLiveLink.Process(sendCallback, data);
    };

    interface->CreateObserver("face_landmarks_with_iris", landmarkCallback);
    interface->Start();

    while (grabFrames) {
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
        const int pressed_key = cv::waitKey(100);
        if (pressed_key >= 0 && pressed_key != 255) grabFrames = false;
    }

    interface->Stop();
    delete interface;
    return 0;
}