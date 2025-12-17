#ifndef NOTE_DETECTION_H
#define NOTE_DETECTION_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

// Notaların kutucuklarını bulur
std::vector<cv::Rect> detectNotes(const cv::Mat& binaryNoStaff, cv::Mat& output);

// -- YENİ EKLENECEKLER --
// Notanın Y merkezini hesaplar
int estimateNoteheadY(const cv::Mat& binaryNoStaff, const cv::Rect& r);

// Y koordinatına göre nota ismini döndürür
std::string getNoteNameFromY(int y, const std::vector<int>& staffLines);

#endif // NOTE_DETECTION_H