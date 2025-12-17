#include "camera_input.h"
#include <iostream>

cv::Mat getFrameFromCamera(int deviceID) {
    static cv::VideoCapture cap;

    if (!cap.isOpened()) {
        std::cout << "📷 Kamera başlatılıyor (ID: " << deviceID << ")..." << std::endl;
        
        // DEĞİŞİKLİK 1: Sadece deviceID değil, cv::CAP_V4L2 parametresini de ekliyoruz.
        // Bu, OpenCV'ye "Linux Video Sürücüsünü Zorla" der.
        cap.open(deviceID, cv::CAP_V4L2);
        
        if (!cap.isOpened()) {
            std::cerr << "⚠️ Hata: Kamera sürücüsü açılamadı!" << std::endl;
            return cv::Mat();
        }

        // DEĞİŞİKLİK 2: Formatı MJPG (Motion JPEG) yapıyoruz.
        // Ham veri (YUYV) çok büyük olduğu için WSL'de tıkanmaya sebep olur.
        cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
        
        // Çözünürlük ayarları
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    }

    cv::Mat frame;
    cap >> frame; // Kare yakala

    if (frame.empty()) {
        // İlk açılışta bazen boş kare gelebilir, bu normaldir.
        // Kullanıcıyı panikletmemek için burayı sessiz geçebiliriz veya log basabiliriz.
        // std::cerr << "⚠️..." 
    }

    return frame;
}