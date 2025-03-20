#include "../src/AudioFile.h"
#include "../src/HighShelfFilter.h"

#pragma once
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		std::cerr << "Usage: " <<  "KeyBright_DSP_Color" << " <inputFile> <outputFile> <color>\n";
		return 1;
	}
	std::string inputFile = argv[1];
	std::string outputFile = argv[2];
	int color = std::stoi(argv[3]);

	std::cout << "Processing: " << inputFile << " -> " << outputFile << std::endl;

	AudioFile sourceFile(inputFile);

	double gainDB = 0.0;
    if (color == -2)
    {
        gainDB = -16.0;
    }
    else if (color == -1)
    {
        gainDB = -12.0;
    }
	else if (color == 0)
	{
		gainDB = -8.0;
	}
	else if (color == 1)
	{
		gainDB = -4.0;
	}
	else if (color == 2)
	{
		gainDB = 0.0;
	}
	else
	{
		std::cerr << "Invalid color value: " << color << std::endl;
		return 1;
	}
    const double cutoffFreq = 1000.0;
    const double Q = 0.707;
    HighShelfFilter hsFilter(sourceFile.getSampleRate(), cutoffFreq, gainDB, Q);

    std::vector<int16_t> outputSamples;
    for (const auto& sample : sourceFile.readSamples())
    {
		auto processedSample = static_cast<int16_t>(hsFilter.process(sample));
        outputSamples.push_back(processedSample);
    }

    bool verbose = true;
    if (verbose)
    {
        std::cout << "Filtered Output: ";
        for (auto& sample : outputSamples)
        {
            std::cout << sample << " ";
        }
        std::cout << std::endl;
    }

	AudioFileData outputData = {};
	outputData.samples = outputSamples;
	outputData.numChannels = sourceFile.getNumChannels();
    outputData.sampleRate = sourceFile.getSampleRate();
    outputData.bitsPerSample = sourceFile.getBitsPerSample();
	outputData.filename = outputFile;
	AudioFile destFile(outputData);

	std::cout << "Color applied. Output saved to " << outputFile << std::endl;
	return 0;
}




