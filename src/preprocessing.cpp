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

    
    cvtColor(input, gray, COLOR_BGR2GRAY);

    
    GaussianBlur(gray, blurred, Size(5, 5), 0);

    
    adaptiveThreshold(
        blurred,
        binary,
        255,
        ADAPTIVE_THRESH_MEAN_C,
        THRESH_BINARY_INV,
        15,
        8
    );

    

    
    Mat grayBGR, blurredBGR, binaryBGR;
    cvtColor(gray, grayBGR, COLOR_GRAY2BGR);
    cvtColor(blurred, blurredBGR, COLOR_GRAY2BGR);
    cvtColor(binary, binaryBGR, COLOR_GRAY2BGR);

    
    Mat debugRow;
    hconcat(vector<Mat>{grayBGR, blurredBGR, binaryBGR}, debugRow);

    namedWindow("Preprocessing Pipeline", WINDOW_NORMAL);
    imshow("Preprocessing Pipeline", debugRow);
    moveWindow("Preprocessing Pipeline", 30, 20);

    cout << "🧪 Debug View: Grayscale | Blurred | Binary" << endl;
    cout << "✅ Preprocessing completed successfully." << endl;

    return binary;
}
