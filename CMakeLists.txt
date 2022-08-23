cmake_minimum_required(VERSION 3.15)

project(MediaPipeLivelink LANGUAGES CXX VERSION 0.1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    "D:/opencv/build/include"
    "D:/opencv/build/include/opencv2"
    "./include"
    "./include/livelink"
    "./include/mediapipe"
    "./include/blendshape"
)

link_directories(
    "D:/onnxruntime/onnxruntime-win-x64-gpu-1.12.0/lib/"
    "D:/opencv/build/x64/vc15/lib/"
    "D:/opencv/build/x64/vc15/bin/"
    "./lib"
)

set (SRC ./main.cpp)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} opencv_world3410 onnxruntime onnxruntime_providers_cuda onnxruntime_providers_shared onnxruntime_providers_tensorrt mediapipe)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)