#include "perspective.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

static vector<Point2f> orderPoints(const vector<Point>& pts)
{
    vector<Point2f> ordered(4);

    vector<Point> p = pts;
    sort(p.begin(), p.end(),
         [](const Point& a, const Point& b) {
             return (a.x + a.y) < (b.x + b.y);
         });

    ordered[0] = p[0]; // top-left
    ordered[2] = p[3]; // bottom-right

    if (p[1].x > p[2].x) {
        ordered[1] = p[1]; // top-right
        ordered[3] = p[2]; // bottom-left
    } else {
        ordered[1] = p[2];
        ordered[3] = p[1];
    }

    return ordered;
}

cv::Mat perspectiveCorrect(const cv::Mat& input)
{
    if (input.empty()) {
        cerr << "❌ perspectiveCorrect: empty input\n";
        return input;
    }

    Mat gray, blur, edges;
    cvtColor(input, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, Size(5,5), 0);
    Canny(blur, edges, 75, 200);

    vector<vector<Point>> contours;
    findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    sort(contours.begin(), contours.end(),
         [](const vector<Point>& a, const vector<Point>& b) {
             return contourArea(a) > contourArea(b);
         });

    vector<Point> page;
    for (const auto& c : contours) {
        double peri = arcLength(c, true);
        vector<Point> approx;
        approxPolyDP(c, approx, 0.02 * peri, true);

        if (approx.size() == 4) {
            page = approx;
            break;
        }
    }

    if (page.size() != 4) {
        cerr << "⚠️ Page contour not found, skipping perspective correction.\n";
        return input.clone();
    }

    vector<Point2f> src = orderPoints(page);

    float w1 = norm(src[0] - src[1]);
    float w2 = norm(src[2] - src[3]);
    float maxW = max(w1, w2);

    float h1 = norm(src[0] - src[3]);
    float h2 = norm(src[1] - src[2]);
    float maxH = max(h1, h2);

    vector<Point2f> dst = {
        Point2f(0, 0),
        Point2f(maxW - 1, 0),
        Point2f(maxW - 1, maxH - 1),
        Point2f(0, maxH - 1)
    };

    Mat M = getPerspectiveTransform(src, dst);
    Mat warped;
    warpPerspective(input, warped, M, Size((int)maxW, (int)maxH));

    imshow("Perspective Corrected", warped);
    return warped;
}
