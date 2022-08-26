#ifndef BLEND_SHAPE_CONFIG_HPP
#define BLEND_SHAPE_CONFIG_HPP

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

// struct PCF {
//     PCF(int near = 1,
//         int far = 10000,
//         int height = 1920,
//         int width = 1080,
//         double fy = 1074.520446598223) : near_(near),
//                                          far_(far),
//                                          height_(height),
//                                          width_(width),
//                                          fy_(fy) {
//         fov_y_ = 2 * std::atan(height / (2 * fy_));
//         auto heightAtNear = 2 * near_ * std::tan(0.5 * fov_y_);
//         auto widthAtNear = width * heightAtNear / height;
//         left_ = -0.5 * widthAtNear;
//         right_ = 0.5 * widthAtNear;
//         bottom_ = -0.5 * heightAtNear;
//         top_ = 0.5 * heightAtNear;
//     }

//     int near_;
//     int far_;
//     int height_;
//     int width_;
//     int fy_;
//     int fov_y_;
//     int left_;
//     int right_;
//     int bottom_;
//     int top_;
// };

}  // namespace LiveLink

#endif