#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct WavHeader {
    char riff[4];               // "RIFF"
    uint32_t chunkSize;         // File size - 8 bytes
    char wave[4];               // "WAVE"
    char fmt[4];                // "fmt "
    uint32_t subchunk1Size;     // 16 for PCM
    uint16_t audioFormat;       // 1 for PCM
    uint16_t numChannels;       // 1 for mono, 2 for stereo
    uint32_t sampleRate;        // Samples per second
    uint32_t byteRate;          // sampleRate * numChannels * bitsPerSample / 8
    uint16_t blockAlign;        // numChannels * bitsPerSample / 8
    uint16_t bitsPerSample;     // 8, 16, 24, or 32
    char data[4];               // "data"
    uint32_t dataSize;          // Num samples * numChannels * bitsPerSample / 8
};
#pragma pack(pop)

