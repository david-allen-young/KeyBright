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
        uint32_t fileSize;
        char wave[4] = {'W', 'A', 'V', 'E'};
        char fmt[4] = {'f', 'm', 't', ' '};
        uint32_t formatSize = 16;
        uint16_t audioFormat = 1;
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
        char data[4] = {'d', 'a', 't', 'a'};
        uint32_t dataSize;
};
#pragma pack(pop)

bool writeWavFile(const std::string& filename, WavHeader_Writer header, const std::vector<int16_t>& samples)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "[ERROR] Cannot open file for writing: " << filename << std::endl;
        return false;
    }

    // Compute actual data size (in bytes)
    header.dataSize = static_cast<uint32_t>(samples.size()) * sizeof(int16_t); // Total audio data size
    header.fileSize = sizeof(WavHeader_Writer) + header.dataSize;            // 44 bytes (header) + dataSize

    // Log values for debugging
    std::cout << "[INFO] Writing WAV file: " << filename << std::endl;
    std::cout << "[INFO] Channels: " << header.numChannels << std::endl;
    std::cout << "[INFO] Sample Rate: " << header.sampleRate << " Hz" << std::endl;
    std::cout << "[INFO] Bits Per Sample: " << header.bitsPerSample << std::endl;
    std::cout << "[INFO] Byte Rate: " << header.byteRate << std::endl;
    std::cout << "[INFO] Block Align: " << header.blockAlign << std::endl;
    std::cout << "[INFO] Data Size: " << header.dataSize << " bytes" << std::endl;
    std::cout << "[INFO] Total File Size: " << header.fileSize << " bytes" << std::endl;

    // Write WAV header (44 bytes)
    file.write(reinterpret_cast<const char*>(&header), sizeof(WavHeader_Writer));

    // Write sample data
    file.write(reinterpret_cast<const char*>(samples.data()), header.dataSize);

    file.close();
    std::cout << "[SUCCESS] WAV file written successfully: " << filename << std::endl;
    return true;
}
