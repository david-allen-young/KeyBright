#include "WavHeader.h"

#pragma once
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
    if (header.audioFormat != 1) // 1 = PCM
    {
        std::cerr << "Error: Unsupported audio format (Only PCM is supported)\n";
        return false;
    }
    size_t numSamples = header.dataSize / (header.bitsPerSample / 8);
    samples.resize(numSamples);
    file.read(reinterpret_cast<char*>(samples.data()), header.dataSize);
    if (file.gcount() < header.dataSize)
    {
        std::cerr << "Warning: File size mismatch. Some samples may be missing.\n";
    }
    file.close();
    return true;
}

bool writeWavFile(const std::string& filename, const WavHeader& header, const std::vector<int16_t>& samples)
{
    std::cout << "writeWavFile() called!" << std::endl; // Debugging message
    if (samples.empty())
    {
        std::cerr << "Warning: Attempting to write an empty file." << std::endl;
    }
    /*TEMP DEBUG*/
    std::string hardcode = "C:\\Users\\chief\\OneDrive\\Documents\\VS Sandbox\\Test_OutputFile.wav";
    /**/
    std::ofstream file(/*filename*/hardcode, std::ios::binary);
    if (!file)
    {
        std::cerr << "Error: Cannot write to file " << /*filename*/hardcode << std::endl;
        return false;
    }
    WavHeader outputHeader = header;
    outputHeader.dataSize = static_cast<uint32_t>(samples.size()) * (outputHeader.bitsPerSample / 8);
    outputHeader.chunkSize = 36 + outputHeader.dataSize;
    std::cout << "Writing WAV header...\n";
    file.write(reinterpret_cast<const char*>(&outputHeader), sizeof(WavHeader));
    std::cout << "Header written.\n";
    std::cout << "Writing sample data...\n";
    file.write(reinterpret_cast<const char*>(samples.data()), outputHeader.dataSize);
    std::cout << "Sample data written.\n";
    file.close();
    file.flush();
    return true;
}