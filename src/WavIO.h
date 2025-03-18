#include "WavHeader.h"
#include <fstream>
#include <iostream>
#include <vector>

bool readWavFile(const std::string& filename, WavHeader& header, std::vector<int16_t>& samples)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }
    file.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));
    if (std::string(header.riff, 4) != "RIFF" || std::string(header.wave, 4) != "WAVE")
    {
        std::cerr << "Error: Invalid WAV file format\n";
        return false;
    }
    size_t numSamples = header.dataSize / (header.bitsPerSample / 8);
    samples.resize(numSamples);
    file.read(reinterpret_cast<char*>(samples.data()), header.dataSize);
    file.close();
    return true;
}

bool writeWavFile(const std::string& filename, const WavHeader& header, const std::vector<int16_t>& samples)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Error: Cannot write to file " << filename << std::endl;
        return false;
    }
    WavHeader outputHeader = header;
    outputHeader.dataSize = samples.size() * sizeof(int16_t);
    outputHeader.chunkSize = outputHeader.dataSize + sizeof(WavHeader) - 8;
    file.write(reinterpret_cast<const char*>(&outputHeader), sizeof(WavHeader));
    file.write(reinterpret_cast<const char*>(samples.data()), outputHeader.dataSize);
    file.close();
    return true;
}