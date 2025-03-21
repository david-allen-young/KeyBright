#pragma once
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

#pragma pack(push, 1)
struct WavHeader_Reader
{
        char riff[4];
        uint32_t fileSize;
        char wave[4];
};
#pragma pack(pop)

bool readWavFile(const std::string& filename, WavHeader_Reader& header, std::vector<int16_t>& samples)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "[ERROR] Cannot open file: " << filename << std::endl;
        return false;
    }

    // Read RIFF header
    file.read(reinterpret_cast<char*>(&header), sizeof(WavHeader_Reader));
    if (std::string(header.riff, 4) != "RIFF" || std::string(header.wave, 4) != "WAVE")
    {
        std::cerr << "[ERROR] Invalid WAV file format\n";
        return false;
    }

    std::cout << "[INFO] Opened WAV file: " << filename << std::endl;
    std::cout << "[INFO] RIFF Header: " << std::string(header.riff, 4) << ", WAVE Header: " << std::string(header.wave, 4) << std::endl;

    // Search for "fmt " and "data" chunks dynamically
    char chunkID[4];
    uint32_t chunkSize;
    bool foundFmt = false, foundData = false;
    uint16_t audioFormat = 0, numChannels = 0, blockAlign = 0, bitsPerSample = 0;
    uint32_t sampleRate = 0, byteRate = 0, dataSize = 0;

    while (file.read(chunkID, 4)) // Read chunk ID
    {
        file.read(reinterpret_cast<char*>(&chunkSize), 4); // Read chunk size
        std::cout << "[INFO] Found Chunk: " << std::string(chunkID, 4) << " | Size: " << chunkSize << " bytes" << std::endl;

        if (std::string(chunkID, 4) == "fmt ")
        {
            foundFmt = true;
            file.read(reinterpret_cast<char*>(&audioFormat), 2);
            file.read(reinterpret_cast<char*>(&numChannels), 2);
            file.read(reinterpret_cast<char*>(&sampleRate), 4);
            file.read(reinterpret_cast<char*>(&byteRate), 4);
            file.read(reinterpret_cast<char*>(&blockAlign), 2);
            file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

            std::cout << "[INFO] Audio Format: " << (audioFormat == 1 ? "PCM" : (audioFormat == 0xFFFE ? "Extensible" : "Unknown")) << std::endl;
            std::cout << "[INFO] Channels: " << numChannels << ", Sample Rate: " << sampleRate << " Hz" << std::endl;
            std::cout << "[INFO] Bits Per Sample: " << bitsPerSample << ", Byte Rate: " << byteRate << std::endl;

            if (chunkSize > 16)
            {
                // Skip any extra fmt chunk bytes safely
                file.seekg(chunkSize - 16, std::ios::cur);
            }
        }
        else if (std::string(chunkID, 4) == "data")
        {
            foundData = true;
            //dataSize = static_cast<uint32_t>(std::filesystem::file_size(filename)) - 44;
            dataSize = chunkSize;

            if (bitsPerSample == 0 || numChannels == 0)
            {
                std::cerr << "[ERROR] bitsPerSample or numChannels not set before data chunk\n";
                return false;
            }

            size_t numSamples = dataSize / (bitsPerSample / 8);
            std::cout << "[INFO] Number of Samples: " << numSamples << std::endl;

            samples.resize(numSamples);
            file.read(reinterpret_cast<char*>(samples.data()), dataSize);
        }
        else
        {
            std::cout << "[INFO] Skipping unknown chunk: " << std::string(chunkID, 4) << std::endl;
            if (chunkSize > 0)
            {
                // Ensure skipping is within bounds
                file.seekg(chunkSize, std::ios::cur);
            }
        }
    }

    if (!foundFmt || !foundData)
    {
        std::cerr << "[ERROR] Missing required WAV chunks\n";
        return false;
    }

    file.close();
    std::cout << "[SUCCESS] WAV file loaded successfully!\n";
    return true;
}
