#ifndef FILE_MANIPULATION_HEADER
#define FILE_MANIPULATION_HEADER

#include <string>
#include <fstream>
#include <sstream>


/**
 * @brief Load a file into a string
 * 
 * @param path Path to file
 * @return Contents of file in a std::string
 */
std::string loadFileToString(std::string path) {
	// Open the file for reading
	std::ifstream file;
	std::stringstream string;
	file.open(path, std::ios::in);
	string << file.rdbuf();
	return string.str();
}
#endif