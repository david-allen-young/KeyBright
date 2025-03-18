#include "../src/WavHeader.h"
#include "../src/WavIO.h"

#include <string>

std::vector<int16_t> pitchShift(const std::vector<int16_t>& samples, float pitchFactor)
{
    std::vector<int16_t> newSamples;
    newSamples.reserve(static_cast<size_t>(samples.size() / pitchFactor));

    for (size_t i = 0; i < newSamples.capacity(); ++i)
    {
        float index = i * pitchFactor;
        size_t idx = static_cast<size_t>(index);
        if (idx + 1 >= samples.size())
            break;

        // Linear interpolation
        float frac = index - idx;
        int16_t sample = static_cast<int16_t>((1 - frac) * samples[idx] + frac * samples[idx + 1]);
        newSamples.push_back(sample);
    }
    return newSamples;
}

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		std::cerr << "Usage: " << "KeyBright_DSP_Pitch" << " <inputFile> <outputFile> <pitchShift>\n";
		return 1;
	}
	std::string inputFile = argv[1];
	std::string outputFile = argv[2];
	int pitchShift = std::stoi(argv[3]);

	// TODO: Add your code here

	std::cout << "Pitch shift applied. Output saved to " << outputFile << std::endl;
	return 0;
}


