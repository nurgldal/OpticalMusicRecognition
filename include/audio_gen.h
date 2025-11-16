#ifndef AUDIO_GEN_H
#define AUDIO_GEN_H

#include <string>
#include <vector>


void generateMelodyFromNotes(const std::vector<std::string>& noteNames,
                             const std::string& outMp3Path);

#endif 
