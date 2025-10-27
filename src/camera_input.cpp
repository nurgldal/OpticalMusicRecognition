#include <opencv2/opencv.hpp>
#include <iostream>

#include "preprocessing.h"
#include "staff_detection.h"
#include "note_detection.h"

using namespace cv;
using namespace std;

int main() {
    Mat frame = imread("../assets/test_frame.jpg");

    if (frame.empty()) {
        cerr << "⚠️ Error: test_frame.jpg could not be loaded!" << endl;
        return -1;
    }

    imshow("Original", frame);

    // Ön işleme
    Mat binary = preprocessImage(frame);

    // Staff detection
    Mat withStaff = detectAndDrawStaffLines(frame, binary);

    // Note detection
    Mat detectedNotes;
    vector<Rect> notes = detectNotes(binary, detectedNotes);

    cout << "✅ Pipeline completed. Total detected notes: " << notes.size() << endl;

    waitKey(0);
    destroyAllWindows();
    return 0;
}
