#include <iostream>
#include "omr_pipeline.h"

int main() {
    std::string inputImage = "../assets/input.jpg";
    std::string outputMp3  = "../output/melody.mp3";


    std::cout << "📂 Input : " << inputImage << std::endl;
    std::cout << "🎵 Output: " << outputMp3 << std::endl;

    if (!runOMR(inputImage, outputMp3)) {
        std::cerr << "❌ OMR failed!" << std::endl;
        return -1;
    }

    std::cout << "✅ OMR completed (FILE MODE)." << std::endl;
    return 0;
}
