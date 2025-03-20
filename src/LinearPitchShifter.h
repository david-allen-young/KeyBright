#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>
#include <iostream>

class LinearPitchShifter
{
public:
	std::vector<int16_t> process(const std::vector<int16_t>& samples, int semitones)
	{
        std::cout << "Applying pitch shift: " << semitones << " semitones (factor: " << pitchFactor(semitones) << ")" << std::endl;
		return applyPitchShift(samples, pitchFactor(semitones));
	}

private:
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
};