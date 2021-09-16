#ifndef FILE_MANIPULATION_HEADER
#define FILE_MANIPULATION_HEADER

#include <string>
#include <fstream>

// NOLINTNEXTLINE
std::string loadFileToString(std::string path) {
	std::ifstream file;
	file.open(path, std::ios::in);
		
	std::string completeString;
	std::string inputLine;
	
	while (std::getline(file, inputLine)) {
		completeString.append(inputLine + "\n");
	}
	return completeString;
}

#endif