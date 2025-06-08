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
        std::cerr << "Input file does not exist: " << inputFile << std::endl;
        return ERROR_NOFILE;
    }
    int errorCode = ERROR_NONE;
    std::string outputFile = argv[2];
    ColorArg colorArg = ColorArg::Neutral;
    if (std::isdigit(*argv[3]))
    {
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
    }
    PitchArg pitchArg = PitchArg::Neutral;
    if (std::isdigit(*argv[4]))
    {
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
    }
    return errorCode;
}

