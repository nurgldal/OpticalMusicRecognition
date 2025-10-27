# 🎵 Real-Time Optical Music Recognition (OMR) with Camera  
## 🇹🇷 Gerçek Zamanlı Kamera ile Nota Tanıma ve MP3 Üretimi (C++)

---

## 🧠 Project Description | Proje Açıklaması
This project implements a **real-time Optical Music Recognition (OMR)** system using **C++** and **OpenCV**.  
Users draw musical notes on paper, and the **camera captures video input**.  
Each frame is processed in real time to **detect staff lines, recognize notes**, and **generate a melody** that can be played or saved as an MP3 file.  

Bu proje, **C++ ve OpenCV** kullanılarak geliştirilen **gerçek zamanlı optik müzik tanıma (OMR)** sistemidir.  
Kullanıcı, notaları kağıda çizer; **kamera bu görüntüyü video olarak alır.**  
Sistem her kareyi işleyerek **nota çizgilerini ve sembollerini tespit eder**, **notaları tanır** ve **melodiyi oluşturup MP3 olarak kaydeder.**

---

## 🎯 Project Goals | Projenin Amaçları
🎥 Real-time video input via camera  
🎼 Automatic staff and note detection  
🎵 Note pitch & duration recognition  
📝 Export results as `detected_notes.txt`  
🔊 Melody synthesis and MP3 export  

🎥 Kameradan gerçek zamanlı video girişi  
🎼 Nota çizgilerinin ve notaların otomatik tespiti  
🎵 Notaların ses perdesi ve süresinin tanınması  
📝 Sonuçların `detected_notes.txt` dosyasına kaydedilmesi  
🔊 Melodinin üretilmesi ve MP3 olarak dışa aktarılması  

---

## ⚙️ Technologies & Libraries | Kullanılan Teknolojiler
- **C++17 / CMake**
- **OpenCV** → Video capture, preprocessing, contour detection  
- **PortAudio / RtAudio** → Real-time sound generation  
- **FFmpeg / LAME** → MP3 conversion and saving  
- **Standard Library (fstream, string)** → Writing note data to `.txt`

---

## 🧩 System Workflow | Sistem Akışı

**English:**
1️⃣ Capture live video stream from the camera  
2️⃣ Extract frames continuously  
3️⃣ Apply preprocessing (grayscale, threshold, denoise)  
4️⃣ Detect staff lines via Hough Transform or projection profile  
5️⃣ Detect note symbols using contour or template matching  
6️⃣ Recognize pitch and duration of each note  
7️⃣ Write results to `output/detected_notes.txt`  
8️⃣ Generate and export the melody as `output/melody.mp3`

**Türkçe:**
1️⃣ Kameradan canlı video akışı alınır  
2️⃣ Kareler (frame) sürekli olarak işlenir  
3️⃣ Görüntü ön işleme (gri tonlama, threshold, gürültü azaltma) yapılır  
4️⃣ Nota çizgileri Hough Transform veya projeksiyon profiliyle bulunur  
5️⃣ Nota sembolleri kontur veya şablon eşleme yöntemiyle tespit edilir  
6️⃣ Her notanın ses perdesi ve süresi tanınır  
7️⃣ Sonuçlar `output/detected_notes.txt` dosyasına yazılır  
8️⃣ Melodi üretilir ve `output/melody.mp3` olarak kaydedilir  

---

## 📁 Project Structure | Proje Yapısı

## 📁 Project Structure | Proje Yapısı

| **File / Folder** | **Description (EN)** | **Açıklama (TR)** |
|--------------------|----------------------|--------------------|
| `OMR-Video/` | Main project directory | Projenin ana dizini |
| ├── `src/` | Contains all source files | Tüm kaynak kod dosyaları |
| │ ├── `main.cpp` | Entry point of the project | Projenin giriş noktası |
| │ ├── `camera_input.cpp` | Captures and processes video frames | Kameradan video karelerini alır ve işler |
| │ ├── `preprocessing.cpp` | Grayscale & perspective correction | Görüntü ön işleme ve perspektif düzeltme |
| │ ├── `staff_detection.cpp` | Detects staff lines | Nota çizgilerini tespit eder |
| │ ├── `note_detection.cpp` | Detects contours & recognizes note pitch | Nota konturlarını ve perdesini tanır |
| │ ├── `audio_gen.cpp` | Generates sound using PortAudio | PortAudio ile ses üretimi yapar |
| │ ├── `file_writer.cpp` | Writes notes to `detected_notes.txt` | Tanınan notaları `detected_notes.txt` dosyasına yazar |
| │ └── `utils.cpp` | Contains helper functions | Yardımcı fonksiyonları içerir |
| ├── `include/` | Header files directory | Başlık (header) dosyalarının bulunduğu klasör |
| │ ├── `camera_input.h` | Camera function declarations | Kamera işlevlerinin bildirimi |
| │ ├── `audio_gen.h` | Audio generation functions | Ses üretimi fonksiyonları |
| │ ├── `note_utils.h` | Note recognition helpers | Nota tanıma yardımcıları |
| │ ├── `file_writer.h` | File writing functions | Dosya yazma işlevleri |
| │ └── `config.h` | Constants and settings | Sabitler ve yapılandırma ayarları |
| ├── `output/` | Output folder | Çıktı dosyalarının tutulduğu klasör |
| │ ├── `detected_notes.txt` | Text output of recognized notes | Tanınan notaların metin çıktısı |
| │ ├── `melody.mp3` | Generated melody file | Oluşturulan melodinin ses dosyası |
| ├── `CMakeLists.txt` | CMake configuration file | CMake yapılandırma dosyası |
| └── `README.md` | Project documentation | Proje açıklama dosyası |


---
## 🧾 Example Output (detected_notes.txt)
| **Note** | **Pitch** | **Duration** | **Start (s)** |
|-----------|------------|---------------|----------------|
| 1 | C4 | 1/4 | 0.0 |
| 2 | D4 | 1/2 | 0.5 |
| 3 | E4 | 1/4 | 1.0 |
| 4 | F4 | 1/4 | 1.5 |
| 5 | G4 | 1/2 | 2.0 |

📝 **Explanation / Açıklama:**  
Each line in `detected_notes.txt` represents a recognized note with its **pitch**, **duration**, and **starting time** (in seconds).  
`detected_notes.txt` dosyasındaki her satır, tanınan bir notayı, onun **perdesi (pitch)**, **süresi (duration)** ve **başlangıç zamanı (saniye)** bilgileriyle birlikte gösterir.