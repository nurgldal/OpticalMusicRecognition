#include "preprocessing.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat preprocessImage(const Mat& input) {
    Mat gray, blurred, binary;

    if (input.empty()) {
        cerr << "⚠️ Error: Input image is empty!" << endl;
        return binary;
    }

    // 1️⃣ Grayscale
    cvtColor(input, gray, COLOR_BGR2GRAY);
    imshow("1 - Grayscale", gray);

    // 2️⃣ Noise reduction (Gaussian Blur)
    GaussianBlur(gray, blurred, Size(5, 5), 0);
    imshow("2 - Blurred", blurred);

    // 3️⃣ Threshold (Binary Image)
    adaptiveThreshold(blurred, binary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 15, 8);
    imshow("3 - Binary (Thresholded)", binary);

    cout << "✅ Preprocessing completed successfully." << endl;
    return binary;
}
