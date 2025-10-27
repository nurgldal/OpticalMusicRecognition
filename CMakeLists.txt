cmake_minimum_required(VERSION 3.10)

project(OMRTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    src
    include
)

add_executable(test_camera
    src/camera_input.cpp
)

target_link_libraries(test_camera
    ${OpenCV_LIBS}
)
