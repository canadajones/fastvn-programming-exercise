#ifndef FILE_MANIPULATION_HEADER
#define FILE_MANIPULATION_HEADER

#include <string>
#include <fstream>
#include <sstream>

// NOLINTNEXTLINE
std::string loadFileToString(std::string path) {
	// Open the file for reading
	std::ifstream file;
	std::stringstream string;
	file.open(path, std::ios::in);
	string << file.rdbuf();
	return string.str();
}
#endif