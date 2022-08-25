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
    "./include/nlohmann"
    "./include/blendshape"
)

link_directories(
    "D:/onnxruntime/onnxruntime-win-x64-gpu-1.12.0/lib/"
    "D:/opencv/build/x64/vc15/lib/"
    "D:/opencv/build/x64/vc15/bin/"
    "./lib"
)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

file(GLOB srcs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

foreach(mainfile IN LISTS srcs)
    get_filename_component(srcname ${mainfile} NAME_WE)
    add_executable(${srcname} ${mainfile})
    target_link_libraries(${srcname}
        opencv_world3410
        onnxruntime
        onnxruntime_providers_cuda
        onnxruntime_providers_shared
        onnxruntime_providers_tensorrt
        mediapipe
    )
endforeach()