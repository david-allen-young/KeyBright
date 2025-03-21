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

// Read WAV file dynamically with logging
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
                file.seekg(chunkSize - 16, std::ios::cur); // Skip any extra fmt chunk bytes safely
            }
        }
        else if (std::string(chunkID, 4) == "data")
        {
            foundData = true;

            // Confirm current file position
            std::streampos posBefore = file.tellg();
            std::cout << "[DEBUG] File position before reading dataSize: " << posBefore << std::endl;

            // Check bytes before we read dataSize to confirm alignment
            char checkBytes[8] = {};
            file.read(checkBytes, 8);

            std::cout << "[DEBUG] Next 8 bytes in file: "
                      << std::hex << static_cast<int>(checkBytes[0]) << " "
                      << static_cast<int>(checkBytes[1]) << " "
                      << static_cast<int>(checkBytes[2]) << " "
                      << static_cast<int>(checkBytes[3]) << " | "
                      << static_cast<int>(checkBytes[4]) << " "
                      << static_cast<int>(checkBytes[5]) << " "
                      << static_cast<int>(checkBytes[6]) << " "
                      << static_cast<int>(checkBytes[7]) << std::dec << std::endl;

            //// Convert first 4 bytes into dataSize manually
            //dataSize = static_cast<uint8_t>(checkBytes[0]) |
            //           (static_cast<uint8_t>(checkBytes[1]) << 8) |
            //           (static_cast<uint8_t>(checkBytes[2]) << 16) |
            //           (static_cast<uint8_t>(checkBytes[3]) << 24);

            //// Hack:
            //// Limit data size to fit source
            //dataSize = static_cast<uint32_t>(std::filesystem::file_size(filename)) - 44;

            dataSize = chunkSize;

            std::streampos posAfter = file.tellg();
            std::cout << "[DEBUG] File position after reading dataSize: " << posAfter << std::endl;

            std::cout << "[INFO] Found Data Chunk: " << dataSize << " bytes (Raw Bytes: "
                      << std::hex << static_cast<int>(checkBytes[0]) << " "
                      << static_cast<int>(checkBytes[1]) << " "
                      << static_cast<int>(checkBytes[2]) << " "
                      << static_cast<int>(checkBytes[3]) << std::dec << ")" << std::endl;

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

            // Ensure skipping is within bounds
            if (chunkSize > 0)
            {
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
