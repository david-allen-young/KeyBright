#include "../src/AudioFile.h"
#include "../src/LinearPitchShifter.h"

#pragma once
#include <string>
#include <filesystem>
#include <iostream>

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
    LinearPitchShifter pitchShifter;
    auto outputSamples = pitchShifter.process(sourceFile.readSamples(), semitones);
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


