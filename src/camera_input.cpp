#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat frame = imread("../assets/test_frame.jpg");

    if (frame.empty()) {
        cerr << "⚠️ Error: test_frame.jpg could not be loaded!" << endl;
        return -1;
    }

    imshow("Test Frame", frame);
    cout << "✅ Image loaded successfully. Press any key to close the window." << endl;

    waitKey(0);
    destroyAllWindows();
    return 0;
}
