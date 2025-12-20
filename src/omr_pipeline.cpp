#include "omr_pipeline.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

#include "preprocessing.h"
#include "staff_detection.h"
#include "note_detection.h"
#include "audio_gen.h"
#include "file_writer.h"
#include "perspective.h"

using namespace cv;
using namespace std;

static int estimateNoteheadY(const cv::Mat& binaryNoStaff, const cv::Rect& r) {
    cv::Mat roi = binaryNoStaff(r).clone();
    if (roi.empty()) return r.y + r.height / 2;

    if (roi.channels() != 1) cv::cvtColor(roi, roi, cv::COLOR_BGR2GRAY);
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
    if (cBot > maxC) { maxC = cBot; bandIdx = 2; }

    int bandStart = (bandIdx == 0 ? 0 : (bandIdx == 1 ? yTopEnd : yMidEnd));
    int bandEnd   = (bandIdx == 0 ? yTopEnd : (bandIdx == 1 ? yMidEnd : h));
    int localY = (bandStart + bandEnd) / 2;

    return r.y + localY;
}

static string getNoteNameFromY(int y, const vector<int>& staffLines) {
    if (staffLines.size() < 5) return "?";

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

    int idx = step + 2; // referans offset
    if (idx < 0 || idx >= (int)names.size()) return "?";
    return names[idx];
}

bool runOMR(const std::string& imagePath,
            const std::string& outMp3Path)
{
    Mat frame = imread(imagePath);
    if (frame.empty()) {
        cerr << "❌ runOMR: cannot read image: " << imagePath << "\n";
        return false;
    }

    // 1) perspective correction
    Mat deskewed = perspectiveCorrect(frame);

    // 2) preprocessing
    Mat binary = preprocessImage(deskewed);

    // 3) staff detect positions
    vector<int> staffLines = detectStaffLinePositions(binary);

    // 4) remove staff lines
    Mat binaryNoStaff = removeStaffLines(binary);

    // 5) detect notes
    Mat detectedNotesVis;
    vector<Rect> notes = detectNotes(binaryNoStaff, detectedNotesVis);

    // 6) name notes
    vector<string> noteNames;
    noteNames.reserve(notes.size());
    for (const auto& r : notes) {
        int cy = estimateNoteheadY(binaryNoStaff, r);
        noteNames.push_back(getNoteNameFromY(cy, staffLines));
    }

    // 7) optional report
    writeDetectedNotesToFile("output/detected_notes.txt", notes, noteNames);

    // 8) audio
    generateMelodyFromNotes(noteNames, outMp3Path);

    return true;
}
