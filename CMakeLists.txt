cmake_minimum_required(VERSION 3.15)

project(MediaPipeLivelink LANGUAGES CXX VERSION 0.1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

set (OpenCV_DIR "D:/OpenCV/build/x64/vc15/lib")
find_package(OpenCV REQUIRED)

include_directories(
    "./include"
    "./include/mediapipe"
    ${OpenCV_INCLUDE_DIRS}
)

link_directories(
    "./lib"
)

file(GLOB srcs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

foreach(mainfile IN LISTS srcs)
    get_filename_component(srcname ${mainfile} NAME_WE)
    add_executable(${srcname} ${mainfile})
    target_link_libraries(${srcname}
        ${OpenCV_LIBS}
        libmediapipe
    )
endforeach()