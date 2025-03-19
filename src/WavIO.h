#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#pragma pack(push, 1)
struct WavHeader
{
        char riff[4];       // "RIFF"
        uint32_t chunkSize; // File size - 8 bytes
        char wave[4];       // "WAVE"
};
#pragma pack(pop)

// Read WAV file dynamically with logging
bool readWavFile(const std::string& filename, WavHeader& header, std::vector<int16_t>& samples)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "[ERROR] Cannot open file: " << filename << std::endl;
        return false;
    }

    // Read RIFF header
    file.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));
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
    uint16_t audioFormat, numChannels, blockAlign, bitsPerSample;
    uint32_t sampleRate, byteRate, dataSize;

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

            file.seekg(chunkSize - 16, std::ios::cur); // Skip remaining fmt chunk bytes if any
        }
        else if (std::string(chunkID, 4) == "data")
        {
            foundData = true;
            file.read(reinterpret_cast<char*>(&dataSize), 4);
            size_t numSamples = dataSize / (bitsPerSample / 8);
            samples.resize(numSamples);
            file.read(reinterpret_cast<char*>(samples.data()), dataSize);

            std::cout << "[INFO] Found Data Chunk: " << dataSize << " bytes" << std::endl;
            std::cout << "[INFO] Number of Samples: " << numSamples << std::endl;
            break;
        }
        else
        {
            std::cout << "[INFO] Skipping unknown chunk: " << std::string(chunkID, 4) << std::endl;
            file.seekg(chunkSize, std::ios::cur); // Skip unknown chunk
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
