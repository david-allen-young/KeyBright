#include "../src/Args.h"
#include "../src/AudioFile.h"
#include "../src/HighShelfFilter.h"
#include "../src/LinearPitchShifter.h"

#include <iostream>
#include <filesystem>

enum ErrorCodes
{
    ERROR_NONE = 0,     // successful
    ERROR_BOUNDS = 1,   // use default(s)
    ERROR_NOFILE = 2,   // terminate
    ERROR_NOARGS = 3,   // terminate
};

int main(int argc, char* argv[])
{
    std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;
    if (argc != 5)
    {
        std::cerr << "Usage: " << "KeyBright_DSP" << " <inputFile> <outputFile> <color> <pitch>\n";
        return ERROR_NOARGS;
    }
    std::string inputFile = argv[1];
    if (!std::filesystem::exists(inputFile))
    {
        std::cerr << "Input file does not exist: " << inputFile << ". Terminating program." << std::endl;
        return ERROR_NOFILE;
    }
    int errorCode = ERROR_NONE;
    std::string outputFile = argv[2];
    ColorArg colorArg = ColorArg::Neutral;
    int argv3 = std::stoi(argv[3]);
    if (argv3 >= -2 && argv3 <= 2)
    {
        colorArg = ColorArg(argv3);
    }
    else
    {
        std::cerr << "Invalid <color> argument. Reverting to default 'Neutral' 0." << std::endl;
        errorCode = ERROR_BOUNDS;
    }
    PitchArg pitchArg = PitchArg::Neutral;
    int argv4 = std::stoi(argv[4]);
    if (argv4 >= -2 && argv4 <= 2)
    {
        pitchArg = PitchArg(argv4);
    }
    else
    {
        std::cerr << "Invalid <pitch> argument. Reverting to default 'Neutral' 0." << std::endl;
        errorCode = ERROR_BOUNDS;
    }
    AudioFile sourceFile(inputFile);
    std::cout << "Processing pitch and color for input file: " << inputFile << std::endl;
    // PITCH =======================================================================
    LinearPitchShifter pitchShifter;
    int semitones = static_cast<int>(pitchArg) * 5;
    auto outputSamples = pitchShifter.process(sourceFile.readSamples(), semitones);
    // COLOR =======================================================================
    double gainDB = 0.0;
    switch (colorArg)
    {
    case ColorArg::Darkest:
        gainDB = -16.0;
        break;
    case ColorArg::Dark:
        gainDB = -12.0;
        break;
    case ColorArg::Neutral:
        gainDB = -8.0;
        break;
    case ColorArg::Bright:
        gainDB = -4.0;
        break;
    case ColorArg::Brightest:
        gainDB = 0.0;
        break;
    default:
        // this path should not be possible
        std::cerr << "BAD COLOR ARG" << std::endl;
        break;
    }
    const double cutoffFreq = 1000.0;
    const double Q = 0.707;
    HighShelfFilter hsFilter(sourceFile.getSampleRate(), cutoffFreq, gainDB, Q);
    for (auto& sample : outputSamples)
    {
        hsFilter.process(sample);
    }
    // FILE ======================================================================
    AudioFileData outputData = {};
    outputData.samples = outputSamples;
    outputData.numChannels = sourceFile.getNumChannels();
    outputData.sampleRate = sourceFile.getSampleRate();
    outputData.bitsPerSample = sourceFile.getBitsPerSample();
    outputData.filename = outputFile;
    AudioFile destFile(outputData);
    std::cout << "Pitch and Color applied. Output saved to " << outputFile << std::endl;
    return errorCode;
}

