#ifndef IMAGE_HEADER
#define IMAGE_HEADER

#include <memory>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>

#include <SDL2/SDL_image.h>

namespace vnpge {

/**
 * @brief Image base class. All image-likes (prerenderered things) should have this as a base class
 * 
 */
class ImageBase {
	std::string url;
	public:
		ImageBase(std::string location) : url(location) {
		};
};
};
#endif