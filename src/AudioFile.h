#pragma once
#include <vector>

struct AudioFile
{
	std::vector<int16_t> samples;
    int numChannels = 1;
	int sampleRate = 48000;
	int bitsPerSample = 16;
};

