#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_pixels.h>
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

// Nullary function that's handy for use as a breakpoint hook. In the debugger, set a breakpoint on it and step until you've returned
// If using gdb, typing 'break brkpoint()' will accomplish this
// NOLINTNEXTLINE(misc-definitions-in-headers)
void brkpoint() {
	// Keeping a copy of this here in case I need some good ol' print-debugging on which execution path is taken
	std::cout << __FILE__ << " " << __LINE__ << std::endl;
	return;
}

namespace vnpge {


// In the interest of documentation:
// Declare things as struct if they could just as easily be constructed as a simple aggregate structure.
// More formally, if the class
// - does not provide any member or static functions, 
// - does not declare any destructors or move/copy constructors,
// - does not substantially check input for correctness in a way that a correctly coded external initialiser could miss 
// then it may be declared as struct.
// Otherwise, declare as class.
// This doesn't actually matter in any way, since all access specifiers must be explicit.
// But it allows for ascertaining whether a class is "simple" at a glance.

enum struct position : uint {
	left,
	right,
	middle_bottom,
	top,
	bottom
};
 

struct AbsolutePosition {
	public:
	int x;
	int y;
	
};

struct RelativePosition {
	public:
	double x;
	double y;
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
	RelativePosition srcPos;
	RelativePosition destPos;
};

SDL_Surface* makeNewSurface(uint w, uint h) {
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	return SDL_CreateRGBSurface(0, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	#else
	return SDL_CreateRGBSurface(0, w, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	#endif
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

using TextBoxCreator = std::function<SDL_Surface* (AbsoluteDimensions, RelativeDimensions)>;
class TextBox {
	private:
	static uint getPtSize(AbsoluteDimensions boxDims) {
		double boxHeight = static_cast<double>(boxDims.h) * 0.25;
		uint ptSize;
		uint numLines = 5;

		// TODO: Evaluate pixel values based on DPI

		while (boxHeight > 40) {
			ptSize = boxHeight/numLines;
			if (ptSize > 60) {
				numLines += 1;
				continue;
			}
			if (ptSize < 15) {
				numLines -= 1;
				continue;
			}

			if (boxHeight/(numLines + 1) < 35) {
				break;
			}
			else {
				numLines += 1;
			}
		}
		numLines += static_cast<uint>(boxDims.h / boxDims.w);
		return static_cast<uint>(std::round(static_cast<double>(boxHeight)/static_cast<double>(numLines)));
	}

	// Shouldn't really change, except for drastic changes in resolution (akin to changing devices)
	PositionMapping posMap = {
		.srcPos = {0, 0},
		.destPos = {0, 0.75}
	};
	
	// Dimensions relative to "screen" dimensions (it's really the surface dimensions, but realistically it's going to be the screen surface)
	RelativeDimensions relDimensions = {.w = 1.0, .h = 0.25};

	// Changes with resolution when resizing window
	std::shared_ptr<SDL_Surface> box;
	std::shared_ptr<SDL_Surface> displayBox;
	
	// May change at an arbitrary time
	DialogueFont font;
	std::string fontName;
	
	// Regenerated every time the frame is changed
	std::shared_ptr<SDL_Surface> textSurface;

	int lines;
	public:
	/**
	 * @brief Construct a new TextBox.
	 * 
	 * @param surfDimensions Dimensions of surface to which this box will be blitted later.
	 * @param font The font to use, in form of a DialogueFont.
	 * @param boxGenerator A function accepting the absolute dimensions of the destination surface, as well as the relative dimensions this box occupies thereon.
	 */
	TextBox(AbsoluteDimensions surfDimensions, std::string font, TextBoxCreator boxGenerator) :
		box{boxGenerator(surfDimensions, relDimensions), SDL_FreeSurface}, displayBox{makeNewSurface(box->w - 48, box->h - 48), SDL_FreeSurface},
		font{font, getPtSize(surfDimensions)}, fontName{font}, textSurface{nullptr}, lines{0} {
			SDL_SetSurfaceBlendMode(displayBox.get(), SDL_BLENDMODE_ADD);
		};


	const PositionMapping& getPosMap() const {
		return posMap;
	}
	SDL_Surface* getBox() const {
		return box.get();
	};
	
	SDL_Surface* getText() const {
		return displayBox.get();
	}

	SDL_Surface* generateDisplayText(std::string text) {

		// TODO: make per-character text colouring a thing
		SDL_Color color = {255, 255, 255, 255};
		textSurface.reset(TTF_RenderUTF8_Blended_Wrapped(font.getFont(), text.c_str(), color, box->w - 48), SDL_FreeSurface);
		SDL_SetSurfaceBlendMode(textSurface.get(), SDL_BLENDMODE_NONE);
		
		// Reset text position upon text change
		lines = 0;
		updateTextPosition();

		return textSurface.get();
	};

	AbsoluteDimensions updateResolution(AbsoluteDimensions surfDimensions, TextBoxCreator boxGenerator) {
		box.reset(boxGenerator(surfDimensions, relDimensions), SDL_FreeSurface);

		displayBox.reset(makeNewSurface(box->w - 48 , box->h - 48 ), SDL_FreeSurface);
		
		font = {fontName, getPtSize(surfDimensions)};
		return {static_cast<uint>(box->w), static_cast<uint>(box->h)};
	};

	void updateTextPosition() {
		int ptSize = getPtSize({.w = static_cast<uint>(box->w), .h = static_cast<uint>(box->h)});
		SDL_Rect rect {
			.x = 0,
			.y = lines * 2 * ptSize,
			.w = 0,
			.h = 0
		};
		SDL_FillRect(displayBox.get(), nullptr, SDL_MapRGBA(displayBox->format, 0, 0, 0, 0));
		SDL_BlitSurface(textSurface.get(), nullptr, displayBox.get(), &rect);
	};

	void incLines() {
		// This only fires if lines has been decremented already
		// It is thus redundant to check if scrollability is of concern.
		if (lines < 0) {
			lines++;
			updateTextPosition();
		}
	}
	void decLines() {
		// Only allow scrolling if the current text does not fit on screen
		// Also disallow further scrolling if the bottom of the text has been reached.
		// We check for this by seeing if the remainder of the text surface height post-scrolling is lesser than the display box height
		int pixelsMoved = lines * 2 * getPtSize({.w = static_cast<uint>(box->w), .h = static_cast<uint>(box->h)});
		
		if (textSurface->h > displayBox->h && textSurface->h + pixelsMoved > displayBox->h) {
			lines--;
			updateTextPosition();
		}
	}
};

/**
 * @brief Image class that stores SDL bitmaps and metadata.
 * Note: this is a "smart" class, so you should only rely on the getters being part of the API, unless otherwise stated.
 */
class Image {
	private:
		std::string url;
		
		std::shared_ptr<SDL_Surface> imageSurface;
	
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
/**
 * @brief Template for creating Characters
 * Required to create Characters and MetaFrames, and by extension, Frames.
 */
struct MetaCharacter {
	// Credit to ThePotatoGuy for the name/idea of this scheme
	public:
		std::string id;
		
		std::string name;
		
		std::unordered_map<std::string, std::string> metaExpressions;
	public:
		/**
		 * @brief Construct a new MetaCharacter.
		 * Template for creating story characters.
		 * @param characterName Name of the character.
		 * @param metaExpressions An unordered map between expression codes and image paths.
		 * @param id A string uniquely identifying this (Meta)Character. Is used to link to (Meta)Frames.
		 */
		MetaCharacter(std::string characterName, const std::unordered_map<std::string, std::string>& metaExpressions, std::string id ) 
		: id(id), name{characterName}, metaExpressions{metaExpressions} {};


		MetaCharacter() = delete;

};


/**
 * @brief Main story character class
 * Contains everything needed to display a character.
 * NOTE: Requires a meta-character template to construct
 */
struct Character {
	public:
		std::string name;
		std::unordered_map<std::string, Image> expressions;
	public:
	/**
	 * @brief Construct a new Character.
	 * 
	 * @param metaCharacter The MetaCharacter template from which the name and expressions will be drawn
	 */
		Character(const MetaCharacter& metaCharacter) : name(metaCharacter.name) {
			for (auto& metaExpression : metaCharacter.metaExpressions) {
				expressions.insert({metaExpression.first,{metaExpression.second}});
			}
		};
		Character() = delete;
};

/**
 * @brief Template for creating Frames.
 *  NOTE: Requires a meta-character template to construct
 */
struct MetaFrame {
	public:
		std::string textDialogue;
		std::string characterID;
		std::string expression;
		PositionMapping position;
		std::string bg;
		
	public:
		/**
		 * @brief Construct a new MetaFrame object
		 * 
		 * @param dialogue Textual dialogue the character will be speaking in this story frame.
		 * @param character MetaCharacter whose ID this MetaFrame will bind to.
		 * @param exp String uniquely identifying expression the story character will have.
		 * @param imgURL Path to background image.
		 */
		MetaFrame(std::string dialogue, MetaCharacter& character, std::string exp, const PositionMapping& posMap, std::string imgURL) 
		: textDialogue{dialogue}, characterID{character.id}, expression{exp}, position(posMap), bg(imgURL) {
		}

		MetaFrame() = delete;
};
/**
 * @brief A representation of an instant or moment in the story.
 * NOTE: Requires a meta-frame to construct.
 */
struct Frame {
	public:
		std::string textDialogue;
		Character& storyCharacter;
		std::string expression;
		PositionMapping position;

		Image bg;
	public:
	/**
	 * @brief Construct a new Frame
	 * 
	 * @param metaFrame The template for the Frame.
	 * @param character A reference to the current Character.
	 * @param img The image to use as a background.
	 */
		Frame(const MetaFrame& metaFrame, Character& character, Image& img) 
		: textDialogue(metaFrame.textDialogue), storyCharacter(character), expression(metaFrame.expression), position(metaFrame.position), bg{img} {};

		Frame() = delete;
};

// TODO: Write up a good summary of the Chapter organisatorial structure.
class Chapter {
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
		std::vector<Image> backgrounds;

		TextBox textBox;
		std::vector<Frame>::iterator curFrame;
	public:
		Chapter(std::string name, const std::vector<MetaCharacter>& metaCharacters, const std::vector<MetaFrame> metaFrames, AbsoluteDimensions pixelDimensions, std::string font, std::function<SDL_Surface* (AbsoluteDimensions, RelativeDimensions)> boxGenerator) 
		: chapterName{name}, storyCharacters{demetaCharacterVec(metaCharacters)}, textBox{pixelDimensions, font, boxGenerator} {
			storyFrames.reserve(metaFrames.size());
			
			std::unordered_map<std::string, Character&> charIDToRefMap;
			std::unordered_map<std::string, Image&> bgPathtoImageMap;
			
			{
				std::vector<Character>::iterator it1 = storyCharacters.begin();
				for (auto& metaChar : metaCharacters) {
					charIDToRefMap.insert({metaChar.id, *it1.base()});
					it1 = std::next(it1);
					
				}
			}
			
			for (auto& metaFrame : metaFrames) {
				if (charIDToRefMap.count(metaFrame.characterID)) {
					
					if (!bgPathtoImageMap.count(metaFrame.bg)) {
						bgPathtoImageMap.insert({metaFrame.bg, backgrounds.emplace_back(metaFrame.bg)});
					}
					
					storyFrames.emplace_back(metaFrame, charIDToRefMap.at(metaFrame.characterID), bgPathtoImageMap.at(metaFrame.bg));
				}
				else {
					throw std::runtime_error("\033[1m\033[31mFatal error: Character ID '" + metaFrame.characterID + "' doesn't exist.\033[37m");
				}
				
			}
			
			curFrame = storyFrames.begin();
			
			textBox.generateDisplayText(curFrame->textDialogue);	
		};

	
	std::vector<Frame>::iterator nextFrame() {
		if (curFrame != storyFrames.end()) {
			curFrame = std::next(curFrame);
		}
		// Note that merging these two if statements causes the program to segfault upon chapter completion
		// There's probably a neater way to fix this, but this is simple and works
		if (curFrame != storyFrames.end()) {
			textBox.generateDisplayText(curFrame->textDialogue);
		}
		return curFrame;
	}

	std::vector<Frame>::iterator prevFrame() {
		if (curFrame != storyFrames.begin()) {
			curFrame = std::prev(curFrame);
			textBox.generateDisplayText(curFrame->textDialogue);
		}
		return curFrame;
	}
	AbsoluteDimensions updateResolution(AbsoluteDimensions surfDimensions, TextBoxCreator boxGenerator) {
		AbsoluteDimensions newDims = textBox.updateResolution(surfDimensions, boxGenerator);
		textBox.generateDisplayText(curFrame->textDialogue);
		return newDims;
	};
};
};
#endif