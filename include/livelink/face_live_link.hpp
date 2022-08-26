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
        // 54, 53, 52, 51
        json data(blendShape_);
        std::ofstream file("res/blendshape/" + std::to_string(index_) + ".json");
        file << data;
        json data1(landmark);
        std::ofstream file1("res/landmark/" + std::to_string(index_++) + ".json");
        file1 << data1;
        for (int i = 0; i < blendShape_.size(); ++i) {
            if (i >= 51 && i <= 54) {
                blendShape_[i] = 0;
            } else {
                std::random_device rd;
                std::mt19937 seed(rd());
                // std::uniform_int_distribution<> gen(0, 255);
                std::uniform_real_distribution<float> gen(0, 1);
                blendShape_[i] = gen(seed);
            }
        }
        // UpdateEyeBlink(landmark);
        // UpdateMouth(landmark);
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
    void UpdateMouth(const Landmark& landmark) {
        auto mouth = json_["mouth"];
        auto eyeIndex = mouth["eye"].get<std::vector<int>>();
        auto mouthIndex = mouth["mouth"].get<std::vector<int>>();

        auto eyeLandmark = GetLandmark(landmark, eyeIndex);
        auto eyeInnerDistance = Distance(eyeLandmark[0], eyeLandmark[1]);
        auto eyeOuterDistance = Distance(eyeLandmark[2], eyeLandmark[3]);

        auto mouthLandmark = GetLandmark(landmark, mouthIndex);
        auto mouthOpen = Distance(mouthLandmark[0], mouthLandmark[1]);
        auto mouthWidth = Distance(mouthLandmark[2], mouthLandmark[3]);

        auto ratioY = mouthOpen / eyeInnerDistance;
        auto ratioX = mouthWidth / eyeOuterDistance;

        // ratioY = Remap(ratioY, mouth["open"]["low"].get<double>(), mouth["open"]["high"].get<double>());
        // ratioX = Remap(ratioX, mouth["shape"]["low"].get<double>(), mouth["shape"]["high"].get<double>());
        // ratioX = (ratioX - 0.3) * 2;
        auto mouthY = Remap(mouthOpen / eyeInnerDistance, mouth["mouthY"]["low"].get<double>(), mouth["mouthY"]["high"].get<double>());
        // auto mouthX = ratioX;
        // auto ratioI = Clamp(Remap(mouthX, 0, 1) * 2 * Remap(mouthY, 0.2, 0.7), 0, 1);
        // auto ratioA = mouthY * 0.4 + mouthY * (1 - ratioI) * 0.6;
        // auto ratioU = mouthY + Remap(1 - ratioI, 0, 0.3) * 0.1;
        // auto ratioE = Remap(ratioU, 0.2, 1) * (1 - ratioI) * 0.3;
        // auto ratioO = (1 - ratioI) * Remap(mouthY, 0.3, 1) * 0.4;
        blendShape_[int(FaceBlendShape::MouthClose)] = -0.3f;  // ??? 传输负数没有效果
        blendShape_[int(FaceBlendShape::MouthLowerDownLeft)] = mouthY;
        blendShape_[int(FaceBlendShape::MouthLowerDownRight)] = mouthY;
        // std::cout << ratioA << " " << ratioE << " " << ratioI << " " << ratioO << " " << ratioU << " " << (mouthY || 0) << std::endl;
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