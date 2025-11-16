#include <opencv2/opencv.hpp>
#include <iostream>

#include "preprocessing.h"
#include "staff_detection.h"
#include "note_detection.h"
#include "file_writer.h"
#include "audio_gen.h"


using namespace cv;
using namespace std;


static int estimateNoteheadY(const cv::Mat& binaryNoStaff, const cv::Rect& r) {
   
    cv::Mat roi = binaryNoStaff(r).clone();
    if (roi.empty()) {
        
        return r.y + r.height / 2;
    }

    if (roi.channels() != 1) {
        cv::cvtColor(roi, roi, cv::COLOR_BGR2GRAY);
    }

    int h = roi.rows;
    if (h <= 0) {
        return r.y + r.height / 2;
    }

    
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


string getNoteNameFromY(int y, const vector<int>& staffLines) {
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
    int idx = step + 2; // 0 → 2: E4

    if (idx < 0 || idx >= (int)names.size()) {
        return "?";
    }
    return names[idx];
}

int main() {
    
    Mat frame = imread("../assets/test_frame.jpg");
    if (frame.empty()) {
        cerr << "⚠️ Error: ../assets/test_frame.jpg could not be loaded!" << endl;
        return -1;
    }

    imshow("Original", frame);

    Mat binary = preprocessImage(frame);

    Mat withStaff = detectAndDrawStaffLines(frame, binary);

    vector<int> staffLines = detectStaffLinePositions(binary);

    Mat binaryNoStaff = removeStaffLines(binary);

    Mat detectedNotesVis;
    vector<Rect> notes = detectNotes(binaryNoStaff, detectedNotesVis);

    cout << "---- NOTE NAMES (Treble clef) ----" << endl;
    vector<string> noteNames;
    for (size_t i = 0; i < notes.size(); ++i) {
        int cx = notes[i].x + notes[i].width / 2;
        int cy = estimateNoteheadY(binaryNoStaff, notes[i]);

        string name = getNoteNameFromY(cy, staffLines);
        noteNames.push_back(name);  

        cout << "Note[" << i << "] center=(" << cx << "," << cy 
            << ") -> " << name << endl;
    }

    writeDetectedNotesToFile("../output/detected_notes.txt", notes, noteNames);
    generateMelodyFromNotes(noteNames, "../output/melody.mp3");

    cout << "✅ Pipeline completed. Total detected notes: " << notes.size() << endl;

    waitKey(0);
    destroyAllWindows();
    return 0;
}
