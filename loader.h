#ifndef GENERIC_LOADER_HEADER
#define GENERIC_LOADER_HEADER

#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>


namespace vnpge {

class ChapterLoader {
	protected:
	std::vector<std::string> paths;
	
	public:
	ChapterLoader(std::vector<std::string> paths) : paths{paths} {}

};

}
#endif
