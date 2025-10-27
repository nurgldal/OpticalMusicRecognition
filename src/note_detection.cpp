#include "note_detection.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

vector<Rect> detectNotes(const Mat& binary, Mat& output) {
    vector<vector<Point>> contours;
    vector<Rect> noteBoxes;

    if (binary.empty()) {
        cerr << "⚠️ Error: Empty image provided to detectNotes!" << endl;
        return noteBoxes;
    }

    // 1️⃣ Konturları bul
    findContours(binary.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 2️⃣ Renkli kopya oluştur (çizim için)
    cvtColor(binary, output, COLOR_GRAY2BGR);

    cout << "📦 Found " << contours.size() << " total contours." << endl;

    // 3️⃣ Filtreleme
    for (size_t i = 0; i < contours.size(); i++) {
        Rect box = boundingRect(contours[i]);

        if (box.area() < 100 || box.area() > 2000)
            continue;

        float aspectRatio = (float)box.width / (float)box.height;
        if (aspectRatio < 0.6 || aspectRatio > 1.5)
            continue;

        noteBoxes.push_back(box);
        rectangle(output, box, Scalar(0, 0, 255), 2);
    }

    cout << "🎵 Detected " << noteBoxes.size() << " potential notes." << endl;
    imshow("Detected Notes", output);
    return noteBoxes;
}

