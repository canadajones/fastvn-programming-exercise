#ifndef VNPGE_CHAPTER_HEADER
#define VNPGE_CHAPTER_HEADER

#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

#include "structures.h"

#include "image.h"
#include "character.h"


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
		MetaFrame(std::string dialogue, MetaCharacter& character, std::string exp, const PositionMapping& posMap, std::string imgURL);

		MetaFrame(std::string dialogue, std::string characterID, std::string exp, const PositionMapping& posMap, std::string imgURL);


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
		Frame(const MetaFrame& metaFrame, Character& character, Image& img);

		Frame() = delete;
};

// TODO: Write up a good summary of the Chapter organisatorial structure.
class Chapter {
	private:
	static const std::vector<Character> demetaCharacterVec(const std::vector<MetaCharacter>& metaCharacters);

	public:
		std::string chapterName;
		std::vector<Character> storyCharacters;
		std::vector<Frame> storyFrames;
		std::vector<Image> backgrounds;

		std::vector<Frame>::iterator curFrame;
        
	public:
		Chapter(std::string name, const std::vector<MetaCharacter>& metaCharacters, const std::vector<MetaFrame> metaFrames);

	
	std::vector<Frame>::iterator nextFrame();

	std::vector<Frame>::iterator prevFrame();
	
};
};
#endif