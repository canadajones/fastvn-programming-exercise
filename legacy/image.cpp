
#include <string>


namespace vnpge {
class Image {
	public:
	std::string path;
	Image(std::string path) : path{path} {};
};
}