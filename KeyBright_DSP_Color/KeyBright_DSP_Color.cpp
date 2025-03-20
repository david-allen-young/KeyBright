#include "../src/WavReader.h"
#include "../src/WavWriter.h"
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

	WavHeader_Reader header_reader;
    AudioFileData sourceFile = {};
    if (!readWavFile(inputFile, header_reader, sourceFile.samples))
    {
        std::cerr << "Failed to read input file: " << inputFile << std::endl;
        return 1;
    }
    std::cout << "Processing: " << inputFile << " -> " << outputFile << std::endl;

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
    HighShelfFilter hsFilter(sourceFile.sampleRate, cutoffFreq, gainDB, Q);

    std::vector<int16_t> outputSamples;

    for (auto& sample : sourceFile.samples)
    {
		auto processedSample = static_cast<int16_t>(hsFilter.process(sample));
        outputSamples.push_back(processedSample);
    }

    bool verbose = false;
    if (verbose)
    {
        std::cout << "Filtered Output: ";
        for (auto& sample : outputSamples)
        {
            std::cout << sample << " ";
        }
        std::cout << std::endl;
    }

    WavHeader_Writer header_writer = {};
    header_writer.numChannels = sourceFile.numChannels;
    header_writer.sampleRate = sourceFile.sampleRate;
    header_writer.bitsPerSample = sourceFile.bitsPerSample;
    header_writer.byteRate = header_writer.sampleRate * header_writer.numChannels * (header_writer.bitsPerSample / 8);
    header_writer.blockAlign = header_writer.numChannels * (header_writer.bitsPerSample / 8);
    header_writer.dataSize = static_cast<uint32_t>(outputSamples.size()) * sizeof(int16_t);
    header_writer.chunkSize = 36 + header_writer.dataSize;
    if (!writeWavFile(outputFile, header_writer, outputSamples))
    {
        std::cerr << "Failed to write output file: " << outputFile << std::endl;
        return 1;
    }

	std::cout << "Color applied. Output saved to " << outputFile << std::endl;
	return 0;
}




