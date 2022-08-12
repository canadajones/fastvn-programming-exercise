#ifndef VNPGE_IMAGE_HEADER
#define VNPGE_IMAGE_HEADER
#include <string>

namespace vnpge {
class Image {
	public:
	std::string path;
	Image(std::string path);
};
}

#endif