#ifndef MEDIAPIPE_INTERFACE_H_
#define MEDIAPIPE_INTERFACE_H_

#include "mediapipe_log.h"
#include <opencv2/opencv.hpp>
#include <functional>

class MediapipeInterface {
public:
    MediapipeInterface() = default;
    virtual ~MediapipeInterface() = default;

    using LandmarkCallback = std::function<void(std::vector<std::vector<double>>& landmarks)>;
    using MatCallback = std::function<void(cv::Mat& frame)>;

    virtual void SetLogger(const std::shared_ptr<MediapipeLogger>& logger) = 0;
    virtual void SetResourceDir(const std::string& path) = 0;
    virtual void SetGraph(const std::string& path) = 0;
    virtual void Detect(const cv::Mat& frame) = 0;
    virtual void CreateObserver(const std::string& name, const LandmarkCallback& callback) = 0;
    virtual void OpenPreview(const MatCallback& callback) = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
};

#endif