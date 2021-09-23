#ifndef CHAPTER_HEADER
#define CHAPTER_HEADER

#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

#include "structures.h"
#include "character.h"
#include "text.h"

namespace vnpge {

// NOLINTNEXTLINE(misc-definitions-in-headers)
SDL_Surface* makeTextBox(AbsoluteDimensions pixelDimensions, RelativeDimensions relDimensions) {
	// Default simple text box
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	SDL_Surface* textBoxSurface = SDL_CreateRGBSurface(0, pixelDimensions.w*relDimensions.w, pixelDimensions.h*relDimensions.h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	#else
	SDL_Surface* textBoxSurface = SDL_CreateRGBSurface(0, pixelDimensions.w*relDimensions.w, pixelDimensions.h*relDimensions.h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	#endif
	
	if (textBoxSurface == nullptr) {
		std::string err = "Surface could not be created! SDL_Error: "; 
		throw std::runtime_error(err.append(SDL_GetError()));
	}

	SDL_Rect textBox = {
		.x = 0,
		.y = 0,
		.w = textBoxSurface->w,
		.h = textBoxSurface->h
	};

	// Fill with a transparent white
	SDL_FillRect(textBoxSurface, &textBox, SDL_MapRGBA(textBoxSurface->format, 0xFF, 0xFF, 0xFF, 0x7F));

	textBox.x += 4;
	textBox.y += 4;
	textBox.w -= 8;
	textBox.h -= 8;

	// Fill with a transparent black inside the other rectangle, thereby creating a white border
	// Note that FillRect does not blend alphas, so this alpha replaces the white's
	// This is actually desirable, since the colours should have different alphas in order to feel equally transparent
	SDL_FillRect(textBoxSurface, &textBox, SDL_MapRGBA(textBoxSurface->format, 0x30, 0x30, 0x30, 0xAF));

	return textBoxSurface;
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
		MetaFrame(std::string dialogue, std::string characterID, std::string exp, const PositionMapping& posMap, std::string imgURL) 
		: textDialogue{dialogue}, characterID{characterID}, expression{exp}, position(posMap), bg(imgURL) {
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
		Chapter(std::string name, const std::vector<MetaCharacter>& metaCharacters, const std::vector<MetaFrame> metaFrames, AbsoluteDimensions pixelDimensions, std::string font, TextBoxCreator boxGenerator = makeTextBox) 
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