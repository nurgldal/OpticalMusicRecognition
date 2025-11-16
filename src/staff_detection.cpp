#include "staff_detection.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace cv;
using namespace std;

Mat detectAndDrawStaffLines(const Mat& original, const Mat& binary) {
    if (original.empty() || binary.empty()) {
        cerr << "⚠️ Error: Input images to detectAndDrawStaffLines are empty!" << endl;
        return original.clone();
    }

    Mat horiz;
    if (binary.channels() == 1)
        horiz = binary.clone();
    else
        cvtColor(binary, horiz, COLOR_BGR2GRAY);

    int horizontal_size = horiz.cols / 30;
    if (horizontal_size < 15) horizontal_size = 15;

    Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontal_size, 1));

    erode(horiz, horiz, horizontalStructure);
    dilate(horiz, horiz, horizontalStructure);

    imshow("Isolated Horizontal Lines", horiz);

    vector<Vec4i> linesP;
    HoughLinesP(horiz, linesP, 1, CV_PI / 180, 50, 50, 10);

    Mat output = original.clone();
    for (const Vec4i& l : linesP) {
        line(output, Point(l[0], l[1]), Point(l[2], l[3]),
             Scalar(0, 255, 0), 2, LINE_AA);
    }

    cout << "✅ Detected " << linesP.size() << " staff line segments." << endl;
    imshow("Staff Lines Overlay", output);
    return output;
}

cv::Mat removeStaffLines(const cv::Mat& binary) {
    if (binary.empty()) return binary.clone();

    Mat img;
    if (binary.channels() == 1)
        img = binary.clone();
    else
        cvtColor(binary, img, COLOR_BGR2GRAY);

    
    int horiz_size = img.cols / 28;
    if (horiz_size < 12) horiz_size = 12;

    Mat Hkernel = getStructuringElement(MORPH_RECT, Size(horiz_size, 1));
    Mat staffMask;
    erode(img, staffMask, Hkernel);
    dilate(staffMask, staffMask, Hkernel);

    imshow("Staff Mask (for removal)", staffMask);

    Mat cleaned = img.clone();
    cleaned.setTo(0, staffMask);

    Mat closeKernel = getStructuringElement(MORPH_ELLIPSE, Size(5,5));
    morphologyEx(cleaned, cleaned, MORPH_CLOSE, closeKernel);

    medianBlur(cleaned, cleaned, 3);

    imshow("Binary without Staff Lines", cleaned);
    return cleaned;
}


vector<int> detectStaffLinePositions(const Mat& binary) {
    vector<int> staffYs;

    if (binary.empty()) {
        cerr << "⚠️ Error: Empty image provided to detectStaffLinePositions!" << endl;
        return staffYs;
    }

    Mat horiz;
    if (binary.channels() == 1)
        horiz = binary.clone();
    else
        cvtColor(binary, horiz, COLOR_BGR2GRAY);

    int horizontal_size = horiz.cols / 30;
    if (horizontal_size < 15) horizontal_size = 15;

    Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontal_size, 1));

    erode(horiz, horiz, horizontalStructure);
    dilate(horiz, horiz, horizontalStructure);

    vector<Vec4i> linesP;
    HoughLinesP(horiz, linesP, 1, CV_PI / 180, 50, 50, 10);

    vector<int> ys;
    ys.reserve(linesP.size());
    for (const auto& l : linesP) {
        int yMid = (l[1] + l[3]) / 2;
        ys.push_back(yMid);
    }

    if (ys.empty()) {
        cerr << "⚠️ No staff lines detected in detectStaffLinePositions." << endl;
        return staffYs;
    }

    sort(ys.begin(), ys.end());

    const int clusterThresh = 5;
    int sum = ys[0];
    int count = 1;

    for (size_t i = 1; i < ys.size(); ++i) {
        if (abs(ys[i] - ys[i - 1]) <= clusterThresh) {
            sum += ys[i];
            count++;
        } else {
            staffYs.push_back(sum / count);
            sum = ys[i];
            count = 1;
        }
    }
    staffYs.push_back(sum / count);

    cout << "Staff line groups detected: " << staffYs.size() << endl;
    cout << "Staff line y-positions: ";
    for (int y : staffYs) cout << y << " ";
    cout << endl;

    return staffYs;
}
