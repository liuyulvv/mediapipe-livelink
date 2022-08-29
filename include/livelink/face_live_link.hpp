#ifndef FACE_LIVE_LINK_HPP
#define FACE_LIVE_LINK_HPP

#include "live_link_base.hpp"
#include "util.hpp"
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

using json = nlohmann::json;

namespace LiveLink {

class FaceLiveLink : public LiveLinkBase {
public:
    FaceLiveLink(const json& config, int size = 61) : LiveLinkBase(config) {
        blendShape_ = std::vector<float>(size, 0);
    }

    virtual ~FaceLiveLink() = default;

    virtual void Process(const SendCallback& callback, const Landmark& landmark) override {
        UpdateEyeBlink(landmark);
        UpdateMouth(landmark);
        UpdateBrow(landmark);
        UpdatePupil(landmark);
        Encode();
        callback(buffer_);
    }

    void Renew(const SendCallback& callback) {
        for (int i = 0; i < blendShape_.size(); ++i) {
            blendShape_[i] = 0;
        }
        Encode();
        callback(buffer_);
    }

private:
    void UpdatePupil(const Landmark& landmark) {
        auto pupil = json_["pupil"];
        auto leftPupilIndex = pupil["left"].get<std::vector<int>>();
        auto rightPupilIndex = pupil["right"].get<std::vector<int>>();
        auto leftPupil = GetLandmark(landmark, leftPupilIndex);
        auto rightPupil = GetLandmark(landmark, rightPupilIndex);

        auto eye = json_["eye"];
        auto leftEyeIndex = eye["left"].get<std::vector<int>>();
        auto rightEyeIndex = eye["right"].get<std::vector<int>>();

        auto& leftEyeOuterCorner = landmark[leftEyeIndex[0]];
        auto& leftEyeInnerCorner = landmark[leftEyeIndex[1]];

        auto& rightEyeOuterCorner = landmark[rightEyeIndex[0]];
        auto& rightEyeInnerCorner = landmark[rightEyeIndex[1]];

        auto leftEyeWidth = Distance(leftEyeOuterCorner, leftEyeInnerCorner);
        auto rightEyeWidth = Distance(rightEyeOuterCorner, rightEyeInnerCorner);

        auto leftEyeMidPoint = Average(leftEyeOuterCorner, leftEyeInnerCorner);
        auto rightEyeMidPoint = Average(rightEyeOuterCorner, rightEyeInnerCorner);

        auto leftDx = leftEyeMidPoint[0] - leftPupil[0][0];
        auto leftDy = leftEyeMidPoint[1] - leftPupil[0][1];

        auto rightDx = rightEyeMidPoint[0] - rightPupil[0][0];
        auto rightDy = rightEyeMidPoint[1] - rightPupil[0][1];

        auto leftRatioX = leftDx / (leftEyeWidth / 2) * 4;
        auto rightRatioX = rightDx / (rightEyeWidth / 2) * 4;

        auto leftRatioY = leftDy / (leftEyeWidth / 4) * 4;
        auto rightRatioY = rightDy / (rightEyeWidth / 4) * 4;
    }

    void UpdateBrow(const Landmark& landmark) {
        auto brow = json_["brow"];
        auto maxRatio = brow["maxRatio"].get<double>();
        auto low = brow["low"].get<double>();
        auto high = brow["high"].get<double>();
        auto leftBrowIndex = brow["left"].get<std::vector<int>>();
        auto rightBrowIndex = brow["right"].get<std::vector<int>>();
        auto leftBrowDistance = GetEyeLidRatio(landmark, leftBrowIndex);
        auto rightBrowDistance = GetEyeLidRatio(landmark, rightBrowIndex);
        auto leftBrowRatio = leftBrowDistance / maxRatio - 1;
        auto rightBrowRatio = leftBrowDistance / maxRatio - 1;
        auto leftRaiseRatio = Remap(leftBrowRatio, low, high);
        auto rightRaiseRatio = Remap(rightBrowRatio, low, high);
        blendShape_[int(FaceBlendShape::BrowOuterUpLeft)] = leftRaiseRatio;
        blendShape_[int(FaceBlendShape::BrowOuterUpRight)] = rightRaiseRatio;
    }

    void UpdateMouth(const Landmark& landmark) {
        auto mouth = json_["mouth"];
        auto eyeIndex = mouth["eye"].get<std::vector<int>>();
        auto mouthIndex = mouth["mouth"].get<std::vector<int>>();

        auto eyeLandmark = GetLandmark(landmark, eyeIndex);
        auto eyeInnerDistance = Distance(eyeLandmark[0], eyeLandmark[1]);

        auto mouthLandmark = GetLandmark(landmark, mouthIndex);
        auto mouthOpen = Distance(mouthLandmark[0], mouthLandmark[1]);

        auto mouthY = Remap(mouthOpen / eyeInnerDistance, mouth["low"].get<double>(), mouth["high"].get<double>());
        blendShape_[int(FaceBlendShape::MouthLowerDownLeft)] = mouthY;
        blendShape_[int(FaceBlendShape::MouthLowerDownRight)] = mouthY;
    }

    void UpdateEyeBlink(const Landmark& landmark) {
        auto eye = json_["eye"];
        auto leftIndex = eye["left"].get<std::vector<int>>();
        auto rightIndex = eye["right"].get<std::vector<int>>();
        auto maxRatio = eye["maxRatio"].get<double>();
        auto low = eye["low"].get<double>();
        auto high = eye["high"].get<double>();
        auto leftRatio = Clamp(GetEyeLidRatio(landmark, leftIndex) / maxRatio, 0, 2);
        auto rightRatio = Clamp(GetEyeLidRatio(landmark, rightIndex) / maxRatio, 0, 2);
        auto leftRes = Remap(leftRatio, low, high);
        auto rightRes = Remap(rightRatio, low, high);
        blendShape_[int(FaceBlendShape::EyeBlinkLeft)] = (1 - leftRes) || 0;
        blendShape_[int(FaceBlendShape::EyeBlinkRight)] = (1 - rightRes) || 0;
    }

    double GetEyeLidRatio(const Landmark& landmark, const std::vector<int>& index) const {
        auto& eyeOuterCorner = landmark[index[0]];
        auto& eyeInnerCorner = landmark[index[1]];
        auto& eyeOuterUpperLid = landmark[index[2]];
        auto& eyeMidUpperLid = landmark[index[3]];
        auto& eyeInnerUpperLid = landmark[index[4]];
        auto& eyeOuterLowerLid = landmark[index[5]];
        auto& eyeMidLowerLid = landmark[index[6]];
        auto& eyeInnerLowerLid = landmark[index[7]];
        auto eyeWidth = Distance(eyeOuterCorner, eyeInnerCorner);
        auto eyeOuterLidDistance = Distance(eyeOuterUpperLid, eyeOuterLowerLid);
        auto eyeMidLidDistance = Distance(eyeMidUpperLid, eyeMidLowerLid);
        auto eyeInnerLidDistance = Distance(eyeInnerUpperLid, eyeInnerLowerLid);
        auto eyeLidAvg = (eyeOuterLidDistance + eyeMidLidDistance + eyeInnerLidDistance) / 3;
        return eyeLidAvg / eyeWidth;
    }

    virtual void Encode() override {
        buffer_.clear();
        Util::ValueToBuffer(6, buffer_, false);  // version
        for (const auto& c : uuid_) {            // uuid
            buffer_.push_back(c);
        }
        Util::ValueToBuffer(int(name_.size()), buffer_, true);  // name length
        for (const auto& c : name_) {                           // name
            buffer_.push_back(c);
        }
        Util::ValueToBuffer(0, buffer_, true);                                          // ???
        Util::ValueToBuffer(0, buffer_, true);                                          // ???
        Util::ValueToBuffer(60, buffer_, true);                                         // fps
        Util::ValueToBuffer(1, buffer_, true);                                          // int(fps/60)
        buffer_.push_back(0x3d);                                                        // blendshape length
        std::for_each(blendShape_.begin(), blendShape_.end(), [&](const float value) {  // blendshape data
            Util::ValueToBuffer(value, buffer_, true);
        });
    }

    BlendShape blendShape_;
    int version_ = 6;
    std::string uuid_ = "$fcaf7061-2964-459b-87a9-ad78290e21e6";
    std::string name_ = "vzan";
    int fps = 60;
    int size_ = 61;
    std::vector<char> buffer_;
    int index_ = 1700;
};

}  // namespace LiveLink

#endif