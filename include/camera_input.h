#ifndef CAMERA_INPUT_H
#define CAMERA_INPUT_H

#include <opencv2/opencv.hpp>

cv::Mat getFrameFromCamera(int deviceID);

#endif // CAMERA_INPUT_H