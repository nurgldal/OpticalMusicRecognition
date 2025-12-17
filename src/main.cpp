#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

// Kendi başlık dosyalarımız
#include "camera_input.h"
#include "preprocessing.h"
#include "staff_detection.h"
#include "note_detection.h"
#include "file_writer.h"
#include "audio_gen.h"

using namespace cv;
using namespace std;

int main() {
    cout << "=================================================" << endl;
    cout << "🎹 OMR: Gerçek Zamanlı Nota Tanıma Sistemi" << endl;
    cout << "-------------------------------------------------" << endl;
    cout << "Kullanım:" << endl;
    cout << " [ESC] : Çıkış yap (Son kareyi kaydetmeden)" << endl;
    cout << " [S]   : Anlık kareyi kaydet, MP3 üret ve Çık" << endl;
    cout << "=================================================" << endl;

    // Son algılanan notaları tutmak için değişkenler
    vector<Rect> finalNotes;
    vector<string> finalNoteNames;
    bool saveAndExit = false;

    while (true) {
        // 1. KAMERA: Görüntü al
        Mat frame = getFrameFromCamera(0); // 0: Laptop kamerası
        if (frame.empty()) {
            cerr << "⚠️ Kamera görüntüsü alınamadı!" << endl;
            break;
        }

        // 2. ÖN İŞLEME: Gri ton ve Binary
        Mat binary = preprocessImage(frame);

        // 3. NOTA ÇİZGİLERİ (STAFF): Tespit et
        vector<int> staffLines = detectStaffLinePositions(binary);

        // Eğer yeterince çizgi bulamazsa (kağıt yoksa) işlemi atla
        if (staffLines.size() < 5) {
            putText(frame, "Kagit Bekleniyor...", Point(20, 40), 
                    FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
            imshow("OMR Live System", frame);
            if (waitKey(1) == 27) break;
            continue;
        }

        // 4. TEMİZLİK: Çizgileri kaldır (Notaları daha iyi bulmak için)
        Mat binaryNoStaff = removeStaffLines(binary);

        // 5. NOTA KUTULARINI BUL
        Mat visualization; // Çizim yapılacak geçici görsel
        vector<Rect> currentNotes = detectNotes(binaryNoStaff, visualization);

        // 6. NOTA İSİMLERİNİ HESAPLA
        // (Her karede tespit edilen notaların isimlerini buluyoruz)
        vector<string> currentNoteNames;
        
        // Orijinal renkli kare üzerine çizim yapmak için kopyasını al
        Mat displayFrame = frame.clone();

        // Notaların olduğu bölgeye yeşil kutu çiz
        for (size_t i = 0; i < currentNotes.size(); ++i) {
            Rect r = currentNotes[i];
            
            // --- Daha önce taşıdığımız fonksiyonları burada kullanıyoruz ---
            int cy = estimateNoteheadY(binaryNoStaff, r); 
            string noteName = getNoteNameFromY(cy, staffLines);
            
            currentNoteNames.push_back(noteName);

            // Ekrana çiz (Kutu ve İsim)
            rectangle(displayFrame, r, Scalar(0, 255, 0), 2);
            putText(displayFrame, noteName, Point(r.x, r.y - 5), 
                    FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 0, 0), 2);
        }

        // Çizgileri de görelim (Mavi çizgiler)
        for (int y : staffLines) {
            line(displayFrame, Point(0, y), Point(displayFrame.cols, y), Scalar(255, 0, 0), 1);
        }

        // Sonucu Göster
        imshow("OMR Live System", displayFrame);
        // imshow("Binary Process", binaryNoStaff); // İstersen ara adımı açabilirsin

        // 7. KLAVYE KONTROLÜ
        int key = waitKey(30); // 30ms bekle
        
        if (key == 27) { // ESC tuşu
            cout << "Çıkış yapılıyor (Kaydedilmedi)..." << endl;
            break; 
        }
        else if (key == 's' || key == 'S') { // 's' tuşu
            // Son algılanan verileri kaydetmek üzere hafızaya al
            finalNotes = currentNotes;
            finalNoteNames = currentNoteNames;
            saveAndExit = true;
            cout << "📸 Görüntü yakalandı! İşleniyor..." << endl;
            break;
        }
    }

    // 8. ÇIKIŞ VE KAYIT (Eğer 's' ile çıkıldıysa)
    if (saveAndExit && !finalNoteNames.empty()) {
        cout << "💾 Dosyalar kaydediliyor..." << endl;
        
        // Output klasörünü kontrol et (Linux/Mac komutu)
        system("mkdir -p ../output"); 

        // TXT Dosyası
        writeDetectedNotesToFile("../output/detected_notes.txt", finalNotes, finalNoteNames);
        cout << "✅ detected_notes.txt oluşturuldu." << endl;

        // MP3 Dosyası
        generateMelodyFromNotes(finalNoteNames, "../output/melody.mp3");
        cout << "✅ melody.mp3 oluşturuldu." << endl;
    } else if (saveAndExit) {
        cout << "⚠️ Hiç nota bulunamadığı için dosya oluşturulmadı." << endl;
    }

    destroyAllWindows();
    return 0;
}