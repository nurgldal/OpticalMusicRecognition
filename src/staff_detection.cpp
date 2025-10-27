#include "staff_detection.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

cv::Mat detectAndDrawStaffLines(const cv::Mat& original, const cv::Mat& binary) {
    if (original.empty() || binary.empty()) {
        cerr << "⚠️ Error: Input images to detectAndDrawStaffLines are empty!" << endl;
        return original.clone();
    }

    // Çalışacağımız kopyaları hazırla
    Mat horiz = binary.clone();

    // 1️⃣ Yatay çizgileri öne çıkarmak için yatay morfolojik kernel oluştur
    // kernel geniş ama ince: örn. 30x1 gibi
    int horizontal_size = horiz.cols / 30; // görüntü genişliğine göre ölçekliyoruz
    if (horizontal_size < 15) horizontal_size = 15; // çok küçük olmasın

    Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontal_size, 1));

    // 2️⃣ Erozyon + Dilasyon ile yatay çizgileri izole et
    erode(horiz, horiz, horizontalStructure);
    dilate(horiz, horiz, horizontalStructure);

    // Bu aşamada horiz içinde çoğunlukla staff çizgileri kalmış olacak
    imshow("Isolated Horizontal Lines", horiz);

    // 3️⃣ Çizgileri bulmak için HoughLinesP kullan
    vector<Vec4i> linesP;
    HoughLinesP(
        horiz,
        linesP,
        1,                // rho resolution
        CV_PI / 180,      // theta resolution
        50,               // threshold (çizgi olma güveni, gerekirse ayarlanır)
        50,               // minLineLength
        10                // maxLineGap
    );

    // 4️⃣ Sonuçları orijinal görüntü üzerine çiz
    Mat output = original.clone();
    for (size_t i = 0; i < linesP.size(); i++) {
        Vec4i l = linesP[i];
        // Yeşil çizgi
        line(output, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 2, LINE_AA);
    }

    cout << "✅ Detected " << linesP.size() << " staff line segments." << endl;

    imshow("Staff Lines Overlay", output);

    return output;
}
