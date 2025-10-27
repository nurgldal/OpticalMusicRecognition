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


add_executable(omr_demo
    src/camera_input.cpp
    src/preprocessing.cpp
    src/staff_detection.cpp
    src/note_detection.cpp
)

target_link_libraries(omr_demo
    ${OpenCV_LIBS}
)