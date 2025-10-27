#include "note_detection.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// helper: büyük kutuları yatayda parçalara böl (örnek: iki nota tek dikdörtgene sıkıştıysa)
static void splitWideBox(const Rect& box, vector<Rect>& outRects) {
    // Eğer kutu çok genişse (en az 2x daha genişse) iki alt-kutuya böl
    if (box.width > box.height * 2) {
        int halfW = box.width / 2;
        Rect left(box.x, box.y, halfW, box.height);
        Rect right(box.x + halfW, box.y, box.width - halfW, box.height);
        outRects.push_back(left);
        outRects.push_back(right);
    } else {
        outRects.push_back(box);
    }
}

vector<Rect> detectNotes(const Mat& binary, Mat& output) {
    vector<vector<Point>> contours;
    vector<Rect> rawBoxes;
    vector<Rect> finalNoteBoxes;

    if (binary.empty()) {
        cerr << "⚠️ Error: Empty image provided to detectNotes!" << endl;
        return finalNoteBoxes;
    }

    // 0) Temizlik: staff çizgisi + nota yapışmalarını biraz ayır
    Mat cleaned = binary.clone();
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    // Opening = erode + dilate : küçük gürültüyü atar + ince bağlantıları koparır
    morphologyEx(cleaned, cleaned, MORPH_OPEN, kernel);

    // 1) Kontur bul (RETR_TREE -> iç konturları da al, çünkü nota kafaları bazen içte)
    findContours(cleaned.clone(), contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

    // 2) Çizim için renkli kopya hazırla (detected notes preview)
    cvtColor(cleaned, output, COLOR_GRAY2BGR);

    cout << "---------------- NOTE DETECTION ----------------" << endl;
    cout << "Total raw contours found: " << contours.size() << endl;

    // 3) Her konturdan bounding box çıkar
    for (size_t i = 0; i < contours.size(); i++) {
        Rect box = boundingRect(contours[i]);
        rawBoxes.push_back(box);
    }

    // 4) Her box üzerinde işlem yap
    for (const Rect& b : rawBoxes) {
        // Aşırı büyük kutuları at (tüm porteyi kapsayan dikdörtgenler vs.)
        // Bunlar gerçek nota değil, debug kutusu bile istemiyoruz
        if (b.area() > 20000) continue;

        // Çok küçük olanları da at (gürültü pikseli gibi)
        if (b.area() < 50) continue;

        // Aşırı ince/garip şekilleri çıkar
        if (b.width < 5 || b.height < 5) continue;

        // Genişlik-yükseklik orantısına bakalım
        float aspect = static_cast<float>(b.width) / static_cast<float>(b.height);
        if (aspect < 0.3f || aspect > 4.0f) {
            // ama bu kutu belki yatayda iki nota içeriyor olabilir.
            // Bunu incelemek için parçala.
            vector<Rect> splitBoxes;
            splitWideBox(b, splitBoxes);

            for (const Rect& sb : splitBoxes) {
                float sAspect = (float)sb.width / (float)sb.height;
                if (sb.area() < 50) continue;
                if (sb.area() > 20000) continue;
                if (sAspect < 0.3f || sAspect > 4.0f) continue;
                finalNoteBoxes.push_back(sb);
            }

            continue;
        }

        // Buraya gelen her şey makul boyutta -> aday nota kutusu
        finalNoteBoxes.push_back(b);
    }

    // 5) Artık finalNoteBoxes içinde "bize gerçekten müzikal olarak önemli olabilecek"
    // kutular var. Bunları çizelim.
    int idx = 0;
    for (const Rect& nb : finalNoteBoxes) {
        // Kırmızı dikdörtgen
        rectangle(output, nb, Scalar(0, 0, 255), 2);

        // Merkez noktası (sarı)
        Point center(
            nb.x + nb.width / 2,
            nb.y + nb.height / 2
        );
        circle(output, center, 3, Scalar(0, 255, 255), FILLED);

        // Debug olarak numara yaz (hangi notanın hangisi olduğunu anlamak için)
        putText(output,
                to_string(idx),
                Point(nb.x, max(nb.y - 3, 0)),
                FONT_HERSHEY_SIMPLEX,
                0.4,
                Scalar(0, 255, 255),
                1,
                LINE_AA);

        idx++;
    }

    // Görsel pencere
    imshow("Detected Notes", output);

    // Debug log
    cout << "Kept " << finalNoteBoxes.size() << " candidate note boxes after filtering/splitting." << endl;
    for (size_t i = 0; i < finalNoteBoxes.size(); i++) {
        const Rect& r = finalNoteBoxes[i];
        cout << " note[" << i << "] box=("
             << r.x << "," << r.y << "," 
             << r.width << "x" << r.height 
             << ") area=" << r.area()
             << endl;
    }
    cout << "------------------------------------------------" << endl;

    return finalNoteBoxes;
}
