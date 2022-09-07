#ifndef BLEND_SHAPE_CONFIG_HPP
#define BLEND_SHAPE_CONFIG_HPP

#include <deque>
#include <numeric>

namespace LiveLink {

enum class FaceBlendShape {
    EyeBlinkLeft,
    EyeLookDownLeft,
    EyeLookInLeft,
    EyeLookOutLeft,
    EyeLookUpLeft,
    EyeSquintLeft,
    EyeWideLeft,
    EyeBlinkRight,
    EyeLookDownRight,
    EyeLookInRight,
    EyeLookOutRight,
    EyeLookUpRight,
    EyeSquintRight,
    EyeWideRight,
    JawForward,
    JawLeft,
    JawRight,
    JawOpen,
    MouthClose,
    MouthFunnel,
    MouthPucker,
    MouthLeft,
    MouthRight,
    MouthSmileLeft,
    MouthSmileRight,
    MouthFrownLeft,
    MouthFrownRight,
    MouthDimpleLeft,
    MouthDimpleRight,
    MouthStretchLeft,
    MouthStretchRight,
    MouthRollLower,
    MouthRollUpper,
    MouthShrugLower,
    MouthShrugUpper,
    MouthPressLeft,
    MouthPressRight,
    MouthLowerDownLeft,
    MouthLowerDownRight,
    MouthUpperUpLeft,
    MouthUpperUpRight,
    BrowDownLeft,
    BrowDownRight,
    BrowInnerUp,
    BrowOuterUpLeft,
    BrowOuterUpRight,
    CheekPuff,
    CheekSquintLeft,
    CheekSquintRight,
    NoseSneerLeft,
    NoseSneerRight,
    TongueOut,
    HeadYaw,
    HeadPitch,
    HeadRoll,
    LeftEyeYaw,
    LeftEyePitch,
    LeftEyeRoll,
    RightEyeYaw,
    RightEyePitch,
    RightEyeRoll
};

class SmoothDeque {
public:
    SmoothDeque() = delete;
    explicit SmoothDeque(int size) : size_(size) {}

    double Push(double value) {
        if (deque_.size() == size_) {
            deque_.pop_front();
        }
        deque_.push_back(value);
        return Mean();
    }

    double Mean() const {
        double sum = std::accumulate(deque_.begin(), deque_.end(), 0.0);
        return sum / deque_.size();
    }

private:
    int size_;
    std::deque<double> deque_;
};

}  // namespace LiveLink

#endif