#include <ios>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <iostream>


#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>

#include <SDL2/SDL_image.h>

#ifndef STRUCTURES_HEADER 
#define STRUCTURES_HEADER


namespace vnpge {
enum position : uint {
	left,
	right,
	middle_bottom,
	top,
	bottom,
};



struct PositionMapping {
	public:
	std::pair<double, double> srcPos;
	uint srcWidth;
	uint srcHeight;

	std::pair<double, double> destPos;
	uint destWidth;
	uint destHeight;
};


/**
 * @brief Image class that stores SDL bitmaps and metadata.
 * Note: this is a "smart" class, so you should only rely on the getters being part of the API, unless otherwise stated.
 */
class Image {
	public:
		std::string url;
		
		std::shared_ptr<SDL_Surface> imageSurface;
		 

		/**
		 * @brief Construct a new Image object from scratch.
		 * Normal constructor to make a new Image. This will load the image pointed to by location, and will assign it to an SDL_Surface
		 * @param location Absolute or (preferably) relative path to image.
		 */
		Image(std::string location) : url(location), imageSurface{IMG_Load(url.c_str()), SDL_FreeSurface} {
			if (imageSurface == nullptr) {
				throw std::runtime_error("\033[1m\033[31mFatal error: Image '" + url + "' could not be loaded.\033[37m");
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

struct Character {
	public:
		std::string name;
		Image charImage;
	public:
		Character(std::string characterName, std::string imageURL) : name{characterName}, charImage{imageURL} {
		}

		Character() = default;

};

struct Frame {
	public:
		std::string textDialogue;
		Character& storyCharacter;
		Image bg;
	public:
		Frame(std::string dialogue, Character& character, Image& img) : textDialogue{dialogue}, storyCharacter{character}, bg(img) {
		}
		Frame(std::string dialogue, Character& character) : textDialogue{dialogue}, storyCharacter{character}, bg(nullptr) {
		}

		Frame() = delete;
};

struct Chapter {
	public:
		std::string chapterName;
		std::vector<Character> storyCharacters;
		std::vector<Frame> storyFrames;
	public:
		Chapter(std::string name, std::vector<Character> characters, std::vector<Frame> frames) : chapterName{name}, storyCharacters{characters}, storyFrames{frames} {
		}

		Chapter() = default;
};
}

#endif