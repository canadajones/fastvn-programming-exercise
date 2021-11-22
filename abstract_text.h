#ifndef TEXTBOX_HEADER
#define TEXTBOX_HEADER

#include <memory>
#include <stdexcept>
#include <functional>


#include "structures.h"



namespace vnpge {

template <typename T>
using TextBoxCreator = std::function<T (AbsoluteDimensions, RelativeDimensions)>;

class DialogueFormatting {
	private:

	// Shouldn't really change, except for drastic changes in resolution (akin to changing devices)
	PositionMapping posMap = {
		.srcPos = {0, 0},
		.destPos = {0, 0.75}
	};

	
	// Dimensions relative to "screen" dimensions 
	RelativeDimensions relDimensions = {.w = 1.0, .h = 0.25};

	// May change at an arbitrary time
	std::string fontName;
	Colour colour;
	

	
	public:
	/**
	 * @brief Construct a new TextBox
	 * 
	 * @param font Font to use for dialogue
	 * @param screenlines Lines of text that fit on screen
	 * @param textColour Name of colour to print text in
	 */
	DialogueFormatting(std::string font, int screenLines, Colour textColour) :
	fontName{font}, colour{textColour} {};


	const PositionMapping& getPosMap() const {
		return posMap;
	};
	
	const RelativeDimensions& getRelDimensions() const {
		return relDimensions;
	};

	const Colour getTextColour() const {
		return colour;
	};
	
};
};


#endif