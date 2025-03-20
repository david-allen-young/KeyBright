#include "../src/AudioFile.h"

#pragma once
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>
#include <filesystem>
#include <iostream>

float pitchFactor(int semitones)
{
    return std::pow(2.0f, static_cast<float>(semitones) / 12.0f);
}

int16_t linearInterpolation(int16_t a, int16_t b, float t)
{
    auto lhs = (1 - t) * a;
    auto rhs = t * b;
    return static_cast<int16_t>(std::round(lhs + rhs));
}

std::vector<int16_t> applyPitchShift(const std::vector<int16_t>& samples, float pitchFactor)
{
    if (samples.empty())
    {
        return {};
    }
    size_t newSampleCount = static_cast<size_t>(samples.size() / pitchFactor);
    std::vector<int16_t> newSamples;
    newSamples.reserve(newSampleCount);
    float gainCompensation = std::sqrt(pitchFactor);
    for (size_t i = 0; i < newSampleCount; ++i)
    {
        float index = i * pitchFactor;
        size_t idx = static_cast<size_t>(index);
        int16_t sample;
        if (idx + 1 >= samples.size())
        {
            int16_t lastSample = samples.back();
            int16_t secondLastSample = samples.size() > 1 ? samples[samples.size() - 2] : lastSample;
            float extrapolationFactor = index - (samples.size() - 1);
            sample = linearInterpolation(secondLastSample, lastSample, extrapolationFactor);
        }
        else
        {
            float t = index - idx;
            sample = linearInterpolation(samples[idx], samples[idx + 1], t);
        }
        sample = static_cast<int16_t>(std::clamp(sample * gainCompensation, -32768.0f, 32767.0f));
        newSamples.push_back(sample);
    }
    return newSamples;
}


int main(int argc, char* argv[])
{
    std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;
	if (argc != 4)
	{
		std::cerr << "Usage: " << "KeyBright_DSP_Pitch" << " <inputFile> <outputFile> <semitones>\n";
		return 1;
	}
	std::string inputFile = argv[1];
	std::string outputFile = argv[2];
	int semitones = std::stoi(argv[3]);
    AudioFile sourceFile(inputFile);
    std::cout << "Processing: " << inputFile << " -> " << outputFile << std::endl;
    std::cout << "Applying pitch shift: " << semitones << " semitones (factor: " << pitchFactor(semitones) << ")" << std::endl;
    auto outputSamples = applyPitchShift(sourceFile.readSamples(), pitchFactor(semitones));
    AudioFileData outputData = {};
    outputData.samples = outputSamples;
    outputData.numChannels = sourceFile.getNumChannels();
    outputData.sampleRate = sourceFile.getSampleRate();
    outputData.bitsPerSample = sourceFile.getBitsPerSample();
    outputData.filename = outputFile;
    AudioFile destFile(outputData);
	std::cout << "Pitch shift applied. Output saved to " << outputFile << std::endl;
	return 0;
}


