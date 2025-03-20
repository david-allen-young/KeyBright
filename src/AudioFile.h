#pragma once
#include <vector>

struct AudioFileData
{
	std::vector<int16_t> samples;
    int numChannels = 1;
	int sampleRate = 48000;
	int bitsPerSample = 16;
	std::string filename;
};

class AudioFile
{
public:
	AudioFile(AudioFileData data) : data(data) {}
	AudioFileData getData() const { return data; }
	const std::vector<int16_t>& readSamples() const { return data.samples; }
    std::vector<int16_t>& writeSamples() { return data.samples; }
	int getNumChannels() const { return data.numChannels; }
	int getSampleRate() const { return data.sampleRate; }
	int getBitsPerSample() const { return data.bitsPerSample; }
private:
	AudioFileData data;
};