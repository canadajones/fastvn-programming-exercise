#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <random>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_ttf.h>



#include <SDL2/SDL_image.h>

#ifndef STRUCTURES_HEADER 
#define STRUCTURES_HEADER

// NOLINTNEXTLINE(misc-definitions-in-headers)
void brkpoint() {
	std::cout << __FILE__ << " " << __LINE__ << std::endl;
	return;
}

namespace vnpge {
enum position : uint {
	left,
	right,
	middle_bottom,
	top,
	bottom,
};

struct AbsolutePosition {
	public:
	double x;
	double y;
};

struct RelativePosition {
	public:
	int x;
	int y;
};

struct AbsoluteDimensions {
	public:
	uint w;
	uint h;
};

struct RelativeDimensions {
	public:
	double w;
	double h;
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

class DialogueFont {
	private:
		std::shared_ptr<TTF_Font> font;

	public:
		DialogueFont(std::string name, uint ptsize) : font{TTF_OpenFontIndex(name.c_str(), ptsize, 0), TTF_CloseFont} {
		}
		const TTF_Font* getFont() const {
			return font.get();
		}
		TTF_Font* getFont() {
			return font.get();
		}
};


class TextBox {
	private:
	// Shouldn't really change, except for drastic changes in resolution (akin to changing devices)
	// The widths and heights in this should be ignored, but attempts should be made to keep them up to date with the surface 

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
	PositionMapping posMap = {
		.srcPos = {0, 0},
		.destPos = {0, 0.75}
	};
	#pragma GCC diagnostic pop 

	// Dimensions relative to "screen" dimensions (it's really the surface dimensions, but realistically it's going to be the screen surface)
	RelativeDimensions relDimensions;

	// Changes with resolution when resizing window
	std::shared_ptr<SDL_Surface> box;
	

	// May change at an arbitrary time
	DialogueFont font;
	
	// Regenerated every time the frame is changed
	std::shared_ptr<SDL_Surface> textSurface;
	
	

	public:
	/**
	 * @brief Construct a new TextBox.
	 * 
	 * @param surfDimensions Dimensions of surface to which this box will be blitted later.
	 * @param font The font to use, in form of a DialogueFont.
	 * @param boxGenerator A function accepting the absolute dimensions of the destination surface, as well as the relative dimensions this box occupies thereon.
	 */
	TextBox(AbsoluteDimensions surfDimensions, std::string font, std::function<SDL_Surface* (AbsoluteDimensions, RelativeDimensions)> boxGenerator) :
		relDimensions{.w = 1.0, .h = 0.25}, box{boxGenerator(surfDimensions, relDimensions), SDL_FreeSurface}, font{font, 40}, textSurface{nullptr} {};


	PositionMapping getPosMap() const {
		return posMap;
	}
	SDL_Surface* getBox() const {
		return box.get();
	};
	
	SDL_Surface* getText() const {
		return textSurface.get();
	}

	SDL_Surface* generateDisplayText(std::string text) {
		SDL_Color color = {255, 255, 255, 255};
		textSurface.reset(TTF_RenderUTF8_Blended(font.getFont(), text.c_str(), color), SDL_FreeSurface);

		return textSurface.get();
	};

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

struct MetaCharacter {
	// Credit to ThePotatoGuy for the name/idea of this scheme
	public:
		std::string id;
		
		std::string name;
		
		std::string imageURL;

	public:
		MetaCharacter(std::string characterName, std::string imageURL, std::string id ) : id(id), name{characterName}, imageURL{imageURL} {
		}

		MetaCharacter() = delete;

};

struct Character {
	public:
		std::string name;
		Image charImage;
	public:
		Character(std::string characterName, std::string imageURL) : name{characterName}, charImage{imageURL} {};
		Character(const MetaCharacter& metaCharacter) : name(metaCharacter.name), charImage(metaCharacter.imageURL) {};

		Character() = default;

};

struct MetaFrame {
	public:
		std::string textDialogue;
		std::string characterID;
		std::string bg;
	public:
		MetaFrame(std::string& dialogue, MetaCharacter& character, Image& img) : textDialogue{dialogue}, characterID{character.id}, bg(img.url) {
		}
		MetaFrame(std::string dialogue, MetaCharacter& character, std::string imgURL) : textDialogue{dialogue}, characterID{character.id}, bg(imgURL) {
		}

		MetaFrame() = delete;
};

struct Frame {
	public:
		std::string textDialogue;
		Character& storyCharacter;
		Image bg;
	public:
		Frame(const MetaFrame& metaFrame, Character& character) : textDialogue(metaFrame.textDialogue), storyCharacter(character), bg({metaFrame.bg}) {};

		Frame() = delete;
};

struct Chapter {
	private:
	static const std::vector<Character> demetaCharacterVec(const std::vector<MetaCharacter>& metaCharacters) {
		std::vector<Character> createdCharacters;
		createdCharacters.reserve(metaCharacters.size());
		
		for (auto& metaChar : metaCharacters) {
			createdCharacters.push_back({metaChar});
		}
		
		return createdCharacters;
	}
	public:
		std::string chapterName;
		std::vector<Character> storyCharacters;
		std::vector<Frame> storyFrames;

		TextBox textBox;
		std::vector<Frame>::iterator it;
	public:
		Chapter(std::string name, const std::vector<MetaCharacter>& metaCharacters, const std::vector<MetaFrame> metaFrames, AbsoluteDimensions pixelDimensions, std::string font, std::function<SDL_Surface* (AbsoluteDimensions, RelativeDimensions)> boxGenerator) : 
		chapterName{name}, storyCharacters{demetaCharacterVec(metaCharacters)}, textBox{pixelDimensions, font, boxGenerator} {
			
			storyFrames.reserve(metaFrames.size());
			std::cout << __FILE__ << " " << __LINE__ << std::endl;
			std::unordered_map<std::string, Character&> charIDToRefMap;
			std::cout << __FILE__ << " " << __LINE__ << std::endl;
			{
				std::vector<Character>::iterator it1 = storyCharacters.begin();
				for (auto& metaChar : metaCharacters) {
					charIDToRefMap.insert({metaChar.id, *it1.base()});
					std::cout << __FILE__ << " " << __LINE__ << std::endl;
					std::next(it1);
				}
			}
			for (auto& metaFrame : metaFrames) {
				if (charIDToRefMap.count(metaFrame.characterID)) {
					std::cout << __FILE__ << " " << __LINE__ << std::endl;
					storyFrames.push_back({metaFrame, charIDToRefMap.at(metaFrame.characterID)});
				}
				
			}
			std::cout << __FILE__ << " " << __LINE__ << std::endl;
			it = storyFrames.begin();
			std::cout << __FILE__ << " " << __LINE__ << std::endl;
			textBox.generateDisplayText(it->textDialogue);	
		};

	
	std::vector<Frame>::iterator nextFrame() {
		if (it != storyFrames.end()) {
			it = std::next(it);
			textBox.generateDisplayText(it->textDialogue);
		}
		return it;
	}

	std::vector<Frame>::iterator prevFrame() {
		if (it != storyFrames.begin()) {
			it = std::prev(it);
			textBox.generateDisplayText(it->textDialogue);
		}
		return it;
	}
};
};
#endif