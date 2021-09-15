#include <vector>
#include <string>
#include <stdexcept>

#include <boost/json.hpp>

#include "chapter.h"

#ifndef GENERIC_LOADER_HEADER
#define GENERIC_LOADER_HEADER



namespace vnpge {
	namespace json = boost::json;
class ChapterLoader {
	private:
	std::vector<std::string> paths;
	
	public:

};

class JSONLoader {
	private:
	std::vector<std::string> paths;
	
	public:
	JSONLoader(std::vector<std::string>& paths) : paths{paths} {
		if (paths.size() < 1) {
			throw std::runtime_error("No paths to JSON files were specified!");
		}
	}

	// Get the latest version of Boost installed!
	// Then, find something to load files into a string
	// Lastly, parse the strings and construct them into a Chapter
};
}
#endif