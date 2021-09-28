#ifndef IMAGE_HEADER
#define IMAGE_HEADER

#include <memory>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>

#include <SDL2/SDL_image.h>



namespace vnpge {

// NOLINTNEXTLINE(misc-definitions-in-headers)
SDL_Surface* makeNewSurface(uint w, uint h) {
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	return SDL_CreateRGBSurface(0, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	#else
	return SDL_CreateRGBSurface(0, w, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	#endif
};

/**
 * @brief Image class that stores SDL bitmaps and metadata.
 * Note: this is a "smart" class, so you should only rely on the getters being part of the API, unless otherwise stated.
 */
class Image {
	private:
		std::string url;
		
		std::shared_ptr<SDL_Surface> imageSurface;

		std::shared_ptr<SDL_Texture> imageTexture;
	public:
		/**
		 * @brief Construct a new Image object from scratch.
		 * Normal constructor to make a new Image. This will load the image pointed to by location, and will assign it to an SDL_Surface
		 * @param location Absolute or (preferably) relative path to image.
		 */
		Image(std::string location) : url(location), imageSurface{IMG_Load(url.c_str()), SDL_FreeSurface} {
			if (imageSurface == nullptr) {
				throw std::runtime_error("\u001b[31mFatal error: Image '" + url + "' could not be loaded.\u001b[0m");
			}
		};

		/**
		 * @brief Construct a new Image object from an existing SDL_Surface.
		 * 
		 * @param surf Pointer to a dynamically allocated SDL_Surface
		 */
		Image(SDL_Surface* surf) : url("undefined"), imageSurface(surf) {
		};

		// Create a function overload to allow for the creation of an Image with a shared_ptr<SDL_Surface> that has a null deleter.
		// Useful for when the surface is automatically managed elsewhere, and deleting it here would cause undefined or erroneous behaviour.
		// Use only if you are completely sure that having a null deleter for the Image won't lead to memory leaks.

		/**
		 * @brief Construct a new Image object from an existing SDL_Surface, without managing the pointer.
		 * 
		 * @param surf Pointer to a SDL_Surface
		 * @param jankyEnabler May be any bool-compatible value, it will just be ignored.
		 */
		Image(SDL_Surface* surf, bool jankyEnabler) : url("undefined"), imageSurface(surf, [](SDL_Surface*){}) {
		};

		Image() = default;

		SDL_Surface* getSurface() {
			return imageSurface.get();
		};

};
};
#endif