#include "AudioFile.h"
#include "../src/WavReader.h"
#include "../src/WavWriter.h"

AudioFile::AudioFile(std::string filename)
{
    if (filename.empty())
    {
		valid = false;
		return;
    }
    data.filename = filename;
    std::cout << "Loading file: " << data.filename << std::endl;
    WavHeader_Reader header_reader = {};
    if (!readWavFile(data.filename, header_reader, data.samples))
    {
        std::cerr << "Failed to read input file: " << data.filename << std::endl;
    }
    else
    {
        std::cout << "Channels: " << data.numChannels << std::endl;
        std::cout << "Sample rate: " << data.sampleRate << std::endl;
        std::cout << "Bits per sample: " << data.bitsPerSample << std::endl;
        std::cout << "Total samples: " << data.samples.size() << std::endl;
        valid = true;
    }
}

AudioFile::AudioFile(AudioFileData data)
    : data(data)
{
    if (data.filename.empty() || data.samples.empty())
    {
		valid = false;
		return;
    }
    std::cout << "Preparing file: " << data.filename << std::endl;
    WavHeader_Writer header_writer = {};
    header_writer.numChannels = data.numChannels;
    header_writer.sampleRate = data.sampleRate;
    header_writer.bitsPerSample = data.bitsPerSample;
    header_writer.byteRate = header_writer.sampleRate * header_writer.numChannels * (header_writer.bitsPerSample / 8);
    header_writer.blockAlign = header_writer.numChannels * (header_writer.bitsPerSample / 8);
    header_writer.dataSize = static_cast<uint32_t>(data.samples.size()) * sizeof(int16_t);
    header_writer.fileSize = 36 + header_writer.dataSize;
    if (!writeWavFile(data.filename, header_writer, data.samples))
    {
        std::cerr << "Failed to write output file: " << data.filename << std::endl;
    }
    else
    {
		valid = true;
    }
}