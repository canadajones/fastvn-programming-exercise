#ifndef FILE_MANIPULATION_HEADER
#define FILE_MANIPULATION_HEADER

#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>


/**
 * @brief Load a file into a string
 * 
 * @param path Path to file
 * @return Contents of file in a std::string
 */
inline std::string loadFileToString(std::string path) {
	// Open the file for reading
	std::ifstream file;
	std::stringstream string;
	file.open(path, std::ios::in);
	
	if (!file.good()) {
		throw std::runtime_error("Failed to open file " + path);
	}
	
	string << file.rdbuf();
	return string.str();
}
#endif