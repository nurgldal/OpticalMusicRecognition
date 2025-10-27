#ifndef NOTE_DETECTION_H
#define NOTE_DETECTION_H

#include <opencv2/opencv.hpp>
#include <vector>

std::vector<cv::Rect> detectNotes(const cv::Mat& binary, cv::Mat& output);

#endif // NOTE_DETECTION_H
