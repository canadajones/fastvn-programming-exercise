#ifndef CHAPTER_HEADER
#define CHAPTER_HEADER

#include <SDL2/SDL_render.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

#include "structures.h"
#include "character.h"
#include "text.h"

namespace vnpge {



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
template <typename ImageClass>
struct Frame {
	public:
		std::string textDialogue;
		Character<ImageClass>& storyCharacter;
		std::string expression;
		PositionMapping position;

		ImageClass bg;
	public:
	/**
	 * @brief Construct a new Frame
	 * 
	 * @param metaFrame The template for the Frame.
	 * @param character A reference to the current Character.
	 * @param img The image to use as a background.
	 */
		Frame(const MetaFrame& metaFrame, Character<ImageClass>& character, ImageClass& img) 
		: textDialogue(metaFrame.textDialogue), storyCharacter(character), expression(metaFrame.expression), position(metaFrame.position), bg{img} {};

		Frame() = delete;
};

// TODO: Write up a good summary of the Chapter organisatorial structure.
// TODO: Make this independent of rendering backend (why does the abstract chapter structure need to know which class backs the images?)
template <typename ImageClass>
class Chapter {
	private:
	static const std::vector<Character<ImageClass>> demetaCharacterVec(const std::vector<MetaCharacter>& metaCharacters) {
		std::vector<Character<ImageClass>> createdCharacters;
		createdCharacters.reserve(metaCharacters.size());
		
		for (auto& metaChar : metaCharacters) {
			createdCharacters.push_back({metaChar});
		}
		
		return createdCharacters;
	}
	public:
		std::string chapterName;
		std::vector<Character<ImageClass>> storyCharacters;
		std::vector<Frame<ImageClass>> storyFrames;
		std::vector<ImageClass> backgrounds;

		TextBox textBox;
		typename std::vector<Frame<ImageClass>>::iterator curFrame;
	public:
		Chapter(std::string name, const std::vector<MetaCharacter>& metaCharacters, const std::vector<MetaFrame> metaFrames,
		AbsoluteDimensions pixelDimensions, std::string font, SDL_Renderer* renderer, TextBoxCreator boxGenerator = makeTextBox) 
		: chapterName{name}, storyCharacters{demetaCharacterVec(metaCharacters)}, textBox{pixelDimensions, font, boxGenerator, renderer} {
			storyFrames.reserve(metaFrames.size());
			
			std::unordered_map<std::string, Character<ImageClass>&> charIDToRefMap;
			std::unordered_map<std::string, ImageClass&> bgPathtoImageMap;
			
			{
				typename std::vector<Character<ImageClass>>::iterator it1 = storyCharacters.begin();
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

	
	typename std::vector<Frame<ImageClass>>::iterator nextFrame() {
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

	typename std::vector<Frame<ImageClass>>::iterator prevFrame() {
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