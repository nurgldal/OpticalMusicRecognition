#include "audio_gen.h"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdio>   
#include <cstdlib>

using namespace std;

static double noteNameToFreq(const std::string& pitch) {
    static const std::map<std::string, double> table = {
        {"C4", 261.63}, {"D4", 293.66}, {"E4", 329.63}, {"F4", 349.23},
        {"G4", 392.00}, {"A4", 440.00}, {"B4", 493.88},

        {"C5", 523.25}, {"D5", 587.33}, {"E5", 659.25}, {"F5", 698.46},
        {"G5", 783.99}, {"A5", 880.00}, {"B5", 987.77}
    };

    auto it = table.find(pitch);
    if (it != table.end())
        return it->second;

    cerr << "⚠️ Unknown pitch: " << pitch << endl;
    return 0.0;
}


static vector<int16_t> synthMelody(const vector<string>& notes,
                                   double secPerNote,
                                   int sampleRate)
{
    vector<int16_t> buffer;
    const double PI2 = 2.0 * M_PI;
    const double AMP = 0.6;
    const double fade = 0.01;

    for (auto& n : notes) {
        double freq = noteNameToFreq(n);
        int total = (int)(secPerNote * sampleRate);
        int fadeSamples = (int)(fade * sampleRate);
        if (fadeSamples * 2 > total) fadeSamples = total / 4;

        for (int i = 0; i < total; i++) {
            double t = (double)i / sampleRate;
            double env = 1.0;

            if (i < fadeSamples) env = (double)i / fadeSamples;
            else if (i > total - fadeSamples) env = (double)(total - i) / fadeSamples;

            double val = sin(PI2 * freq * t) * AMP * env;
            buffer.push_back((int16_t)(val * 32767.0));
        }
    }

    return buffer;
}

void generateMelodyFromNotes(const vector<string>& noteNames,
                             const string& outMp3Path)
{
    if (noteNames.empty()) {
        cerr << "⚠️ No notes, MP3 not created.\n";
        return;
    }

    const int sampleRate = 44100;
    const double durationPerNote = 0.5;

    vector<int16_t> pcm = synthMelody(noteNames, durationPerNote, sampleRate);

    cout << "🎧 Generating MP3 directly without WAV...\n";

    
    string cmd = "ffmpeg -y -loglevel error -f s16le -ar 44100 -ac 1 -i - \"" + outMp3Path + "\"";
    FILE* ffmpeg = popen(cmd.c_str(), "w");
    if (!ffmpeg) {
        cerr << "❌ Failed to start ffmpeg.\n";
        return;
    }

    fwrite(pcm.data(), sizeof(int16_t), pcm.size(), ffmpeg);
    pclose(ffmpeg);

    cout << "✅ MP3 saved: " << outMp3Path << endl;
}
