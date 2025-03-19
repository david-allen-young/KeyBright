#pragma once
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#pragma pack(push, 1)
struct WavHeader_Writer
{
        char riff[4] = {'R', 'I', 'F', 'F'};
        uint32_t chunkSize; // File size - 8 bytes
        char wave[4] = {'W', 'A', 'V', 'E'};
        char fmt[4] = {'f', 'm', 't', ' '};
        uint32_t subchunk1Size = 16; // PCM = 16 bytes
        uint16_t audioFormat = 1;    // PCM = 1
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
        char data[4] = {'d', 'a', 't', 'a'};
        uint32_t dataSize; // Num samples * numChannels * bitsPerSample / 8
};
#pragma pack(pop)

bool writeWavFile(const std::string& filename, const WavHeader_Writer& header, const std::vector<int16_t>& samples)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "[ERROR] Cannot open file for writing: " << filename << std::endl;
        return false;
    }

    // Compute actual file sizes
    uint32_t fileSize = 36 + header.dataSize;                   // 36 bytes (header) + data size
    WavHeader_Writer modifiedHeader = header;                   // Copy header to modify it
    modifiedHeader.chunkSize = fileSize - 8;                    // RIFF chunkSize = fileSize - 8
    modifiedHeader.dataSize = samples.size() * sizeof(int16_t); // Num samples * bytes per sample

    // Write header
    file.write(reinterpret_cast<const char*>(&modifiedHeader), sizeof(WavHeader_Writer));

    // Write sample data
    file.write(reinterpret_cast<const char*>(samples.data()), modifiedHeader.dataSize);

    file.close();
    std::cout << "[SUCCESS] WAV file written successfully: " << filename << std::endl;
    return true;
}
