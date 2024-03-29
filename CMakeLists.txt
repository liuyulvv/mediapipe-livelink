cmake_minimum_required(VERSION 3.15)

project(MediaPipeLivelink LANGUAGES CXX VERSION 0.1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

if (MSVC)
    set(OpenCV_DIR "C:/Program Files/OpenCV/x64/vc15/lib")
endif()

find_package(OpenCV REQUIRED)

include_directories(
    "./include"
    "./include/mediapipe"
    ${OpenCV_INCLUDE_DIRS}
)

link_directories(
    "./lib"
if (APPLE)
    "bazel-out/darwin_arm64-dbg/bin/mediapipe/library"
endif()
)

file(GLOB srcs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

foreach(mainfile IN LISTS srcs)
    get_filename_component(srcname ${mainfile} NAME_WE)
    add_executable(${srcname} ${mainfile})
    target_link_libraries(${srcname}
        ${OpenCV_LIBS}
        mediapipe
    )
endforeach()