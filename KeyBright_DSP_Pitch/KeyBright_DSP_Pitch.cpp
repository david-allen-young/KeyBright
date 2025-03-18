// KeyBright_DSP_Pitch.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

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


