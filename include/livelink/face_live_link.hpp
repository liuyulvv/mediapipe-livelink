#ifndef FACE_LIVE_LINK_HPP
#define FACE_LIVE_LINK_HPP

#include "live_link_base.hpp"
#include "util.hpp"
#include <fstream>
#include <random>
#include <vector>

using json = nlohmann::json;

namespace LiveLink {

class FaceLiveLink : public LiveLinkBase {
public:
    FaceLiveLink(const json& config, int size = 61, int smoothSize = 4) : LiveLinkBase(config), size_(size), smoothSize_(smoothSize) {
        blendShape_ = std::vector<float>(size_, 0);
        smoothBlendShape_ = std::vector<SmoothDeque>(size_, SmoothDeque(smoothSize_));
    }

    virtual ~FaceLiveLink() = default;

    virtual void Process(const SendCallback& callback, const Landmark& landmark) override {
        UpdateEye(landmark);
        UpdateMouth(landmark);
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
        auto upperOuterLip = GetLandmark(landmark, mouth["upperOuterLip"].get<int>());

        auto mouthCornerLeft = GetLandmark(landmark, mouth["mouthCornerLeft"].get<int>());
        auto mouthCornerRight = GetLandmark(landmark, mouth["mouthCornerRight"].get<int>());
        auto mouthWidth = Distance(mouthCornerLeft, mouthCornerRight);

        auto upperLip = GetLandmark(landmark, mouth["upperLip"].get<int>());
        auto lowerLip = GetLandmark(landmark, mouth["lowerLip"].get<int>());
        std::vector<double> mouthCenter{
            (upperLip[0] + lowerLip[0]) / 2,
            (upperLip[1] + lowerLip[1]) / 2,
            (upperLip[2] + lowerLip[2]) / 2,
            (upperLip[3] + lowerLip[3]) / 2,
            (upperLip[4] + lowerLip[4]) / 2,
        };
        auto mouthOpenDistance = Distance(upperLip, lowerLip);

        auto noseTip = GetLandmark(landmark, mouth["noseTip"].get<int>());
        auto mouthCenterNoseDistance = Distance(mouthCenter, noseTip);

        auto lowestChin = GetLandmark(landmark, mouth["lowestChin"].get<int>());
        auto jawNoseDist = Distance(lowestChin, noseTip);

        auto upperHead = GetLandmark(landmark, mouth["upperHead"].get<int>());
        auto headHeight = Distance(upperHead, lowestChin);

        auto jawOpenRatio = jawNoseDist / headHeight;
        auto jawOpenLow = mouth["jawOpenLow"].get<double>();
        auto jawOpenHigh = mouth["jawOpenHigh"].get<double>();
        UpdateBlendShape(FaceBlendShape::JawOpen, Remap(jawOpenRatio, jawOpenLow, jawOpenHigh));
        // UpdateBlendShape(FaceBlendShape::MouthClose, mouthCenterNoseDistance - mouthOpenDistance);

        auto smileLeft = upperLip[1] - mouthCornerLeft[1];
        auto smileRight = upperLip[1] - mouthCornerRight[1];
        auto smileLow = mouth["smileLow"].get<double>();
        auto smileHigh = mouth["smileHigh"].get<double>();

        auto mouthSmileLeft = 1 - Remap(smileLeft, smileLow, smileHigh);
        auto mouthSmileRight = 1 - Remap(smileRight, smileLow, smileHigh);
        UpdateBlendShape(FaceBlendShape::MouthSmileLeft, mouthSmileLeft);
        UpdateBlendShape(FaceBlendShape::MouthSmileRight, mouthSmileRight);

        UpdateBlendShape(FaceBlendShape::MouthDimpleLeft, mouthSmileLeft * 2 / 3);
        UpdateBlendShape(FaceBlendShape::MouthDimpleRight, mouthSmileRight * 2 / 3);

        auto mouthFrownLeft = mouthCornerLeft[1] - GetLandmark(landmark, mouth["mouthFrownLeft"].get<int>())[1];
        auto mouthFrownRight = mouthCornerRight[1] - GetLandmark(landmark, mouth["mouthFrownRight"].get<int>())[1];
        auto mouthFrownLow = mouth["mouthFrownLow"].get<double>();
        auto mouthFrownHigh = mouth["mouthFrownHigh"].get<double>();
        UpdateBlendShape(FaceBlendShape::MouthFrownLeft, 1 - Remap(mouthFrownLeft, mouthFrownLow, mouthFrownHigh));
        UpdateBlendShape(FaceBlendShape::MouthFrownRight, 1 - Remap(mouthFrownRight, mouthFrownLow, mouthFrownHigh));

        auto mouthLeftStretchLandmark = GetLandmark(landmark, mouth["mouthLeftStretch"].get<int>());
        auto mouthRightStretchLandmark = GetLandmark(landmark, mouth["mouthRightStretch"].get<int>());
        auto mouthLeftStretch = mouthCornerLeft[0] - mouthLeftStretchLandmark[0];
        auto mouthRightStretch = mouthRightStretchLandmark[0] - mouthCornerRight[0];
        auto mouthCenterLeftStretch = mouthCenter[0] - mouthLeftStretchLandmark[0];
        auto mouthCenterRightStretch = mouthCenter[0] - mouthRightStretchLandmark[0];

        auto mouthLeft = Remap(mouthCenterLeftStretch, mouth["mouthLeftLow"].get<double>(), mouth["mouthLeftHigh"].get<double>());
        auto mouthRight = 1 - Remap(mouthCenterRightStretch, mouth["mouthRightLow"].get<double>(), mouth["mouthRightHigh"].get<double>());
        // UpdateBlendShape(FaceBlendShape::MouthLeft, mouthLeft);
        // UpdateBlendShape(FaceBlendShape::MouthRight, mouthRight);

        auto stretchNormalLeft = -0.7 + 0.42 * mouthSmileLeft + 0.36 * mouthLeft;
        auto stretchMaxLeft = -0.45 + 0.45 * mouthSmileLeft + 0.36 * mouthLeft;
        auto stretchNormalRight = -0.7 + 0.42 * mouthSmileRight + 0.36 * mouthRight;
        auto stretchMaxRight = -0.45 + 0.45 * mouthSmileRight + 0.36 * mouthRight;

        UpdateBlendShape(FaceBlendShape::MouthStretchLeft, Remap(mouthLeftStretch, stretchNormalLeft, stretchMaxLeft));
        UpdateBlendShape(FaceBlendShape::MouthStretchRight, Remap(mouthRightStretch, stretchNormalRight, stretchMaxRight));
    }

    void UpdateEye(const Landmark& landmark) {
        // EYE BLINK
        auto eye = json_["eye"];
        auto leftIndex = eye["left"].get<std::vector<int>>();
        auto rightIndex = eye["right"].get<std::vector<int>>();
        auto maxRatio = eye["maxRatio"].get<double>();
        auto leftRatio = GetEyeOpenRatio(landmark, leftIndex, maxRatio);
        auto rightRatio = GetEyeOpenRatio(landmark, rightIndex, maxRatio);
        auto eyeLow = eye["low"].get<double>();
        auto eyeHigh = eye["high"].get<double>();
        auto leftBlink = 1 - Remap(leftRatio, eyeLow, eyeHigh);
        auto rightBlink = 1 - Remap(rightRatio, eyeLow, eyeHigh);
        UpdateBlendShape(FaceBlendShape::EyeBlinkLeft, leftBlink, false);
        UpdateBlendShape(FaceBlendShape::EyeBlinkRight, rightBlink, false);

        // EYE SQUINT
        auto squint = json_["squint"];
        auto squintLeftIndex = squint["left"].get<std::vector<int>>();
        auto squintRightIndex = squint["right"].get<std::vector<int>>();
        auto squintLeftLandmark = GetLandmark(landmark, squintLeftIndex);
        auto squintRightLandmark = GetLandmark(landmark, squintRightIndex);
        auto squintLeft = Distance(squintLeftLandmark[0], squintLeftLandmark[1]);
        auto squintRight = Distance(squintRightLandmark[0], squintRightLandmark[1]);
        auto squintLow = squint["low"].get<double>();
        auto squintHigh = squint["high"].get<double>();
        UpdateBlendShape(FaceBlendShape::EyeSquintLeft, 1 - Remap(squintLeft, squintLow, squintHigh));
        UpdateBlendShape(FaceBlendShape::EyeSquintRight, 1 - Remap(squintRight, squintLow, squintHigh));

        // BROW
        auto brow = json_["brow"];
        auto leftBrowLower = GetLandmark(landmark, brow["leftLower"].get<std::vector<int>>());
        auto rightBrowLower = GetLandmark(landmark, brow["rightLower"].get<std::vector<int>>());
        std::vector<double> leftBrowLowerMean{
            (leftBrowLower[0][0] + leftBrowLower[1][0] + leftBrowLower[2][0]) / 3,
            (leftBrowLower[0][1] + leftBrowLower[1][1] + leftBrowLower[2][1]) / 3,
            (leftBrowLower[0][2] + leftBrowLower[1][2] + leftBrowLower[2][2]) / 3,
            (leftBrowLower[0][3] + leftBrowLower[1][3] + leftBrowLower[2][3]) / 3,
            (leftBrowLower[0][4] + leftBrowLower[1][4] + leftBrowLower[2][4]) / 3,
        };
        std::vector<double> rightBrowLowerMean{
            (rightBrowLower[0][0] + rightBrowLower[1][0] + rightBrowLower[2][0]) / 3,
            (rightBrowLower[0][1] + rightBrowLower[1][1] + rightBrowLower[2][1]) / 3,
            (rightBrowLower[0][2] + rightBrowLower[1][2] + rightBrowLower[2][2]) / 3,
            (rightBrowLower[0][3] + rightBrowLower[1][3] + rightBrowLower[2][3]) / 3,
            (rightBrowLower[0][4] + rightBrowLower[1][4] + rightBrowLower[2][4]) / 3,
        };
        auto leftBrowDistance = Distance(leftBrowLowerMean, GetLandmark(landmark, brow["left"].get<int>()));
        auto rightBrowDistance = Distance(rightBrowLowerMean, GetLandmark(landmark, brow["right"].get<int>()));

        auto browDownLow = brow["browDownLow"].get<double>();
        auto browDownHigh = brow["browDownHigh"].get<double>();

        UpdateBlendShape(FaceBlendShape::BrowDownLeft, 1 - Remap(leftBrowDistance, browDownLow, browDownHigh));
        UpdateBlendShape(FaceBlendShape::BrowDownRight, 1 - Remap(rightBrowDistance, browDownLow, browDownHigh));
        // chose up or down
        UpdateBlendShape(FaceBlendShape::BrowDownLeft, Remap(leftBrowDistance, browDownLow, browDownHigh));
        UpdateBlendShape(FaceBlendShape::BrowDownRight, Remap(rightBrowDistance, browDownLow, browDownHigh));

        auto browOuterUpLow = brow["browOuterUpLow"].get<double>();
        auto browOuterUpHigh = brow["browOuterUpHigh"].get<double>();
        UpdateBlendShape(FaceBlendShape::BrowOuterUpLeft, Remap(leftBrowDistance, browOuterUpLow, browOuterUpHigh));
        UpdateBlendShape(FaceBlendShape::BrowOuterUpRight, Remap(rightBrowDistance, browOuterUpLow, browOuterUpHigh));

        auto innerBrow = GetLandmark(landmark, brow["innerBrow"].get<int>());
        auto upperNose = GetLandmark(landmark, brow["upperNose"].get<int>());
        auto innerBrowDistance = Distance(upperNose, innerBrow);
        auto browInnerUpLow = brow["browInnerUpLow"].get<double>();
        auto browInnerUpHigh = brow["browInnerUpHigh"].get<double>();
        UpdateBlendShape(FaceBlendShape::BrowInnerUp, Remap(innerBrowDistance, browInnerUpLow, browInnerUpHigh));

        auto cheekSquintLeftIndex = brow["cheekSquintLeft"].get<std::vector<int>>();
        auto cheekSquintRightIndex = brow["cheekSquintRight"].get<std::vector<int>>();

        auto cheekSquintLeft = Distance(GetLandmark(landmark, cheekSquintLeftIndex[0]), GetLandmark(landmark, cheekSquintLeftIndex[1]));
        auto cheekSquintRight = Distance(GetLandmark(landmark, cheekSquintRightIndex[0]), GetLandmark(landmark, cheekSquintRightIndex[1]));

        auto cheekSquintLow = brow["cheekSquintLow"].get<double>();
        auto cheekSquintHigh = brow["cheekSquintHigh"].get<double>();
        UpdateBlendShape(FaceBlendShape::CheekSquintLeft, 1 - Remap(cheekSquintLeft, cheekSquintLow, cheekSquintHigh));
        UpdateBlendShape(FaceBlendShape::CheekSquintRight, 1 - Remap(cheekSquintRight, cheekSquintLow, cheekSquintHigh));

        // Nose
        // UpdateBlendShape(FaceBlendShape::NoseSneerLeft, 1 - Remap(cheekSquintLeft, cheekSquintLow, cheekSquintHigh));
        // UpdateBlendShape(FaceBlendShape::NoseSneerRight, 1 - Remap(cheekSquintRight, cheekSquintLow, cheekSquintHigh));
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

    double GetEyeOpenRatio(const Landmark& landmark, const std::vector<int>& index, double maxRatio) const {
        auto eyeDistance = GetEyeLidDistance(landmark, index);
        return Clamp(eyeDistance / maxRatio, 0, 2);
    }

    double GetEyeLidDistance(const Landmark& landmark, const std::vector<int>& index) const {
        double eyeWidth = Distance(GetLandmark(landmark, index[0]), GetLandmark(landmark, index[1]));
        double eyeOuterLid = Distance(GetLandmark(landmark, index[2]), GetLandmark(landmark, index[5]));
        double eyeMidLid = Distance(GetLandmark(landmark, index[3]), GetLandmark(landmark, index[6]));
        double eyeInnerLid = Distance(GetLandmark(landmark, index[4]), GetLandmark(landmark, index[7]));
        double eyeLidAvg = (eyeOuterLid + eyeMidLid + eyeInnerLid) / 3;
        return eyeLidAvg / eyeWidth;
    }

    void UpdateBlendShape(const FaceBlendShape& index, double value, bool smooth = true) {
        if (smooth) {
            blendShape_[int(index)] = smoothBlendShape_[int(index)].Push(value);
        } else {
            blendShape_[int(index)] = value;
        }
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
    SmoothBlendShape smoothBlendShape_;
    int version_ = 6;
    std::string uuid_ = "$fcaf7061-2964-459b-87a9-ad78290e21e6";
    std::string name_ = "liuyulvv";
    int fps = 60;
    int size_ = 61;
    std::vector<char> buffer_;
    int smoothSize_ = 4;
};

}  // namespace LiveLink

#endif