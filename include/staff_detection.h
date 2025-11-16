#ifndef STAFF_DETECTION_H
#define STAFF_DETECTION_H

#include <opencv2/opencv.hpp>
#include <vector>


cv::Mat detectAndDrawStaffLines(const cv::Mat& original, const cv::Mat& binary);


cv::Mat removeStaffLines(const cv::Mat& binary);

std::vector<int> detectStaffLinePositions(const cv::Mat& binary);

#endif 
