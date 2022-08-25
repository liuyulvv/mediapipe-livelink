#ifndef FACE_LIVE_LINK_HPP
#define FACE_LIVE_LINK_HPP

#include "live_link_base.hpp"
#include "util.hpp"
#include <iostream>
#include <vector>

namespace LiveLink {

class FaceLiveLink : public LiveLinkBase {
public:
    FaceLiveLink(const json& config, int size = 61) : LiveLinkBase(config) {
        blendShape_ = std::vector<float>(size, 0);
    }

    virtual ~FaceLiveLink() = default;

    virtual void Process(const SendCallback& callback, const Landmark& landmark) override {
        UpdateEyeOpen(landmark);
        Encode();
        callback(buffer_);
    }

private:
    void UpdateEyeOpen(const Landmark& landmark) {
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
};

}  // namespace LiveLink

#endif