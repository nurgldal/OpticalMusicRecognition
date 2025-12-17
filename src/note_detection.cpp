#include "note_detection.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

int estimateNoteheadY(const cv::Mat& binaryNoStaff, const cv::Rect& r) {
    cv::Mat roi = binaryNoStaff(r).clone();
    if (roi.empty()) return r.y + r.height / 2;

    if (roi.channels() != 1) {
        cv::cvtColor(roi, roi, cv::COLOR_BGR2GRAY);
    }

    int h = roi.rows;
    if (h <= 0) return r.y + r.height / 2;

    int bandH = std::max(3, h / 3);
    int yTopEnd = std::min(bandH, h);
    int yMidEnd = std::min(2 * bandH, h);

    cv::Mat topBand    = roi(cv::Range(0, yTopEnd), cv::Range::all());
    cv::Mat midBand    = roi(cv::Range(yTopEnd, yMidEnd), cv::Range::all());
    cv::Mat bottomBand = roi(cv::Range(yMidEnd, h), cv::Range::all());

    int cTop = cv::countNonZero(topBand);
    int cMid = cv::countNonZero(midBand);
    int cBot = cv::countNonZero(bottomBand);

    int bandIdx = 0;
    int maxC = cTop;
    if (cMid > maxC) { maxC = cMid; bandIdx = 1; }
    if (cBot > maxC) { bandIdx = 2; }

    int bandStart = (bandIdx == 0 ? 0       : (bandIdx == 1 ? yTopEnd : yMidEnd));
    int bandEnd   = (bandIdx == 0 ? yTopEnd : (bandIdx == 1 ? yMidEnd : h));

    int localY = (bandStart + bandEnd) / 2;  
    return r.y + localY;                     
}

std::string getNoteNameFromY(int y, const std::vector<int>& staffLines) {
    if (staffLines.size() < 5) {
        return "?";
    }

    vector<int> lines = staffLines;
    sort(lines.begin(), lines.end()); 

    int topLine = lines[0];      
    int bottomLine = lines[4];   
    
    float spacing = (float)(bottomLine - topLine) / 4.0f;
    if (spacing <= 0.0f) return "?";

    float rel = ((float)bottomLine - (float)y) / (spacing / 2.0f);
    int step = (int)std::round(rel);  

    static const vector<string> names = {
        "C4","D4","E4","F4","G4","A4","B4","C5",
        "D5","E5","F5","G5","A5"
    };
    int idx = step + 2; 

    if (idx < 0 || idx >= (int)names.size()) {
        return "?";
    }
    return names[idx];
}

static Rect mergeRect(const Rect& a, const Rect& b) {
    int x1 = std::min(a.x, b.x);
    int y1 = std::min(a.y, b.y);
    int x2 = std::max(a.x + a.width,  b.x + b.width);
    int y2 = std::max(a.y + a.height, b.y + b.height);
    return Rect(x1, y1, x2 - x1, y2 - y1);
}

vector<Rect> detectNotes(const Mat& binaryNoStaff, Mat& output)
{
    vector<Rect> noteBoxes;

    if (binaryNoStaff.empty()) {
        cerr << "⚠️ detectNotes: empty image!" << endl;
        return noteBoxes;
    }

    Mat work;
    if (binaryNoStaff.channels() == 1)
        work = binaryNoStaff.clone();
    else
        cvtColor(binaryNoStaff, work, COLOR_BGR2GRAY);

    int imgW = work.cols;
    int imgH = work.rows;

    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(work, work, MORPH_OPEN, kernel);

    vector<vector<Point>> contours;
    findContours(work.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // cout << "----- SIMPLE NOTE DETECTION -----" << endl;
    // cout << "Total raw contours found: " << contours.size() << endl;

    vector<Rect> candidates;
    for (const auto& c : contours)
    {
        Rect b = boundingRect(c);
        int area = b.area();
        int cx   = b.x + b.width / 2;

        
        if (area < 35)
            continue;

        
        if (cx < imgW * 0.20)   
            continue;
        
        double contArea = contourArea(c);
        double per      = arcLength(c, true);
        double circ     = 0.0;

        if (per > 0.0)
            circ = 4.0 * CV_PI * contArea / (per * per);

        
        if (circ < 0.12)
            continue;

        candidates.push_back(b);
    }

    // cout << "Candidates after simple filter: " << candidates.size() << endl;

    if (candidates.empty()) {
        cerr << "⚠️ No candidate note boxes after filtering!" << endl;
        return noteBoxes;
    }

    sort(candidates.begin(), candidates.end(),
         [](const Rect& a, const Rect& b) {
             int cxa = a.x + a.width / 2;
             int cxb = b.x + b.width / 2;
             return cxa < cxb;
         });

    vector<Rect> merged;
    Rect current = candidates[0];
    int currentCx = current.x + current.width / 2;

    int mergeThresh = imgW / 20; 

    for (size_t i = 1; i < candidates.size(); ++i) {
        Rect b   = candidates[i];
        int cx   = b.x + b.width / 2;

        if (std::abs(cx - currentCx) <= mergeThresh) {
            current   = mergeRect(current, b);
            currentCx = current.x + current.width / 2;
        } else {
            merged.push_back(current);
            current   = b;
            currentCx = cx;
        }
    }
    merged.push_back(current);

    if (!merged.empty()) {
        vector<int> heights;
        for (const Rect& r : merged) heights.push_back(r.height);
        sort(heights.begin(), heights.end());
        int medianH = heights[heights.size() / 2];

        for (const Rect& r : merged) {
            if (r.height > medianH * 2.0f)
                continue;

            noteBoxes.push_back(r);
        }
    }

    // cout << "Merged note boxes: " << noteBoxes.size() << endl;

    if (noteBoxes.empty()) {
        cerr << "⚠️ All merged boxes filtered out!" << endl;
        return noteBoxes;
    }

    sort(noteBoxes.begin(), noteBoxes.end(),
         [](const Rect& a, const Rect& b) {
             int cxa = a.x + a.width / 2;
             int cxb = b.x + b.width / 2;
             return cxa < cxb;
         });

    
    Mat colored;
    cvtColor(work, colored, COLOR_GRAY2BGR);

    int idx = 0;
    for (const Rect& nb : noteBoxes)
    {
        int cx = nb.x + nb.width / 2;
        int cy = nb.y + static_cast<int>(nb.height * 0.75f);  // biraz aşağı kaydır

        rectangle(colored, nb, Scalar(0,255,0), 1);
        circle(colored, Point(cx,cy), 3, Scalar(0,0,255), -1);

        putText(colored, to_string(idx),
                Point(cx+4, cy-4),
                FONT_HERSHEY_SIMPLEX, 0.4,
                Scalar(0,255,255), 1);

        idx++;
    }

    output = colored;
    imshow("Detected Notes", output);

    //cout << "✅ Kept " << noteBoxes.size() << " note boxes (simple test_frame mode)." << endl;
    //cout << "----------------------------------------" << endl;

    // yukarıdaki cout'lar debug için
    // video 30fps olduğunda her saniye 30 tane mesajla terminali doldurmak istemiyoruz

    // \r : Satır başına dön
    // \033[K : Satırın geri kalanını temizle (önceki uzun yazılar kalmasın diye)
    cout << "\r🎹 [DURUM] Ham Kontur: " << contours.size() 
         << " | Aday: " << candidates.size() 
         << " | ✅ Tespit Edilen Nota: " << noteBoxes.size() 
         << "   " << std::flush;


    return noteBoxes;
}
