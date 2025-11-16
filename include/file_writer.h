#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

bool writeDetectedNotesToFile(const std::string& filename,
                              const std::vector<cv::Rect>& notes,
                              const std::vector<std::string>& noteNames);

#endif 
