#pragma once
#include <cstdint>
#include <vector>
#include <string>

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
    AudioFile(std::string filename);
    AudioFile(AudioFileData data);
	AudioFileData getData() const { return data; }
	const std::vector<int16_t>& readSamples() const { return data.samples; }
    std::vector<int16_t>& writeSamples() { return data.samples; }
	int getNumChannels() const { return data.numChannels; }
	int getSampleRate() const { return data.sampleRate; }
	int getBitsPerSample() const { return data.bitsPerSample; }
	std::string getFilename() const { return data.filename; }
	bool isValid() const { return valid; }
private:
    AudioFileData data = {};
	bool valid = false;
};