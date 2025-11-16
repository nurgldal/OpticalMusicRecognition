#include "file_writer.h"
#include <fstream>
#include <iostream>

bool writeDetectedNotesToFile(const std::string& filename,
                              const std::vector<cv::Rect>& notes,
                              const std::vector<std::string>& noteNames)
{
    std::ofstream out(filename);
    if (!out.is_open())
    {
        std::cerr << "⚠️ Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }

    out << "Detected Notes\n";
    out << "-----------------------------\n";
    out << "Total Notes: " << notes.size() << "\n\n";

    for (size_t i = 0; i < notes.size(); ++i)
    {
        const cv::Rect& r = notes[i];
        out << "Note[" << i << "]: " 
            << "Pitch = " << noteNames[i]
            << ", Box = (" << r.x << "," << r.y 
            << ", " << r.width << "x" << r.height << ")"
            << "\n";
    }

    out.close();
    std::cout << "📁 Notes successfully written to: " << filename << std::endl;
    return true;
}
