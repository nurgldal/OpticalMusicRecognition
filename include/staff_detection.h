#ifndef STAFF_DETECTION_H
#define STAFF_DETECTION_H

#include <opencv2/opencv.hpp>
#include <vector>

// Detect staff lines and draw them on a copy of the original image.
// Returns an image with the detected staff lines highlighted.
cv::Mat detectAndDrawStaffLines(const cv::Mat& original, const cv::Mat& binary);

#endif // STAFF_DETECTION_H
