// KeyBright_DSP_Color.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

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

	// TODO: Add your code here

	std::cout << "Color applied. Output saved to " << outputFile << std::endl;
	return 0;
}

