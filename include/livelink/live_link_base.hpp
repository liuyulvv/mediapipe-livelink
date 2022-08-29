#ifndef LIVE_LINK_BASE_HPP
#define LIVE_LINK_BASE_HPP

#include "blend_shape_config.hpp"
#include "nlohmann/json.hpp"
#include <functional>
#include <opencv.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace LiveLink {

class LiveLinkBase {
public:
    using SendCallback = std::function<void(const std::vector<char>& buffer)>;
    using Landmark = std::vector<std::vector<double>>;
    using BlendShape = std::vector<float>;

    explicit LiveLinkBase(const json& config) : json_(config) {}
    virtual ~LiveLinkBase() = default;

    virtual void Process(const SendCallback& callback, const Landmark& landmark) = 0;
    virtual void Encode() = 0;

    Landmark GetLandmark(const Landmark& landmark, std::vector<int>& index) {
        Landmark ret;
        for (const auto& i : index) {
            ret.push_back(landmark[i]);
        }
        return std::move(ret);
    }

    std::vector<double> Average(const std::vector<double>& x, const std::vector<double>& y) {
        std::vector<double> ret;
        for (int i = 0; i < x.size(); ++i) {
            ret.push_back((x[i] + y[i]) / 2);
        }
        return std::move(ret);
    }

    double Distance(const std::vector<double>& x, const std::vector<double>& y, bool threeD = false) const {
        if (threeD) {
            return std::pow(std::pow(x[0] - y[0], 2) + std::pow(x[1] - y[1], 2) + std::pow(x[2] - y[2], 2), 0.5);
        } else {
            return std::pow(std::pow(x[0] - y[0], 2) + std::pow(x[1] - y[1], 2), 0.5);
        }
    }

    double Clamp(double value, double min, double max) const {
        return std::max(std::min(value, max), min);
    }

    double Remap(double value, double min, double max) const {
        return (Clamp(value, min, max) - min) / (max - min);
    }

    // void CalculateRotation(const Landmark& landmark, const PCF& pcf) {
    //     double width = pcf.width_;
    //     double height = pcf.height_;
    //     double focalLength = width;
    //     std::vector<double> center{width / 2, height / 2};
    //     std::vector<std::vector<double>> cameraMatrix{
    //         {focalLength, 0, center[0]},
    //         {0, focalLength, center[1]},
    //         {0, 0, 1}};
    //     std::vector<std::vector<int>> distCoeff{{0}, {0}, {0}, {0}};

    //     Landmark landmarks;
    //     for (int j = 0; j < 3; ++j) {
    //         std::vector<double> temp;
    //         for (int i = 0; i < 468; ++i) {
    //             temp.push_back({landmark[i][j]});
    //         }
    //         landmarks.push_back(temp);
    //     }
    //     // auto landmarksMat = ToMat(landmarks);
    // }

    // void GetMetricLandmarks(Landmark& landmarks, const PCF& pcf) {
    //     auto landmarksMat = ProjectXY(landmarks, pcf);
    //     auto res = landmarksMat.row(2);
    // }
    //
    // cv::Mat ProjectXY(Landmark& landmarks, const PCF& pcf) {
    //     // landmarks: 3 * 468
    //     assert(landmarks.size() == 3 && landmarks[0].size() == 468);
    //     auto xScale = pcf.right_ - pcf.left_;
    //     auto yScale = pcf.top_ - pcf.bottom_;
    //     auto xTranslation = pcf.left_;
    //     auto yTranslation = pcf.bottom_;
    //     for (int i = 0; i < 468; ++i) {
    //         landmarks[1][i] = 1.0 - landmarks[1][i];
    //     }
    //     auto mat = ToMat(landmarks);
    //     cv::Mat mat1(1, 3, cv::DataType<double>::type);
    //     mat1.at<double>(0, 0) = xScale;
    //     mat1.at<double>(0, 1) = yScale;
    //     mat1.at<double>(0, 2) = xScale;
    //     mat = mat * mat1.t();
    //     cv::Mat mat2(1, 3, cv::DataType<double>::type);
    //     mat2.at<double>(0, 0) = xTranslation;
    //     mat2.at<double>(0, 1) = yTranslation;
    //     mat2.at<double>(0, 2) = 0;
    //     mat = mat + mat2.t();
    //     return std::move(mat);
    // }
    //
    // cv::Mat ToMat(const std::vector<std::vector<double>>& value) {
    //     cv::Mat mat(value.size(), value.at(0).size(), cv::DataType<double>::type);
    //     for (int i = 0; i < mat.rows; ++i) {
    //         for (int j = 0; j < mat.cols; ++j) {
    //             mat.at<double>(i, j) = value.at(i).at(j);
    //         }
    //     }
    //     return std::move(mat);
    // }
    //
    // std::vector<std::vector<double>> ToVector(const cv::Mat& mat) {
    //     std::vector<std::vector<double>> ret(mat.rows, std::vector<double>(mat.cols, 0));
    //     for (int i = 0; i < mat.rows; ++i) {
    //         for (int j = 0; j < mat.cols; ++j) {
    //             ret.at(i).at(j) = mat.at<double>(i, j);
    //         }
    //     }
    //     return ret;
    // }

protected:
    const json& json_;
};

}  // namespace LiveLink

#endif