#ifndef TEXTBOX_HEADER
#define TEXTBOX_HEADER

#include <memory>
#include <stdexcept>
#include <functional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>

#include <SDL2/SDL_image.h>

#include <SDL2/SDL_ttf.h>

#include "structures.h"



namespace vnpge {

template <typename T>
using TextBoxCreator = std::function<T (AbsoluteDimensions, RelativeDimensions)>;

class TextBoxInfo {
	private:

	// Shouldn't really change, except for drastic changes in resolution (akin to changing devices)
	PositionMapping posMap = {
		.srcPos = {0, 0},
		.destPos = {0, 0.75}
	};

	int screenLines;
	
	// Dimensions relative to "screen" dimensions 
	RelativeDimensions relDimensions = {.w = 1.0, .h = 0.25};

	// May change at an arbitrary time
	std::string fontName;
	std::string colour;
	
	// Regenerated every time the frame is changed
	int linesScrolled;

	
	public:
	/**
	 * @brief Construct a new TextBox
	 * 
	 * @param font Font to use for dialogue
	 * @param screenlines Lines of text that fit on screen
	 * @param textColour Name of colour to print text in
	 */
	TextBoxInfo(std::string font, int screenLines, std::string textColour) :
	screenLines{screenLines}, fontName{font}, colour{textColour}, linesScrolled{0} {};


	const PositionMapping& getPosMap() const {
		return posMap;
	}
	
	const RelativeDimensions& getRelDimensions() const {
		return relDimensions;
	}

	/**
	 * @brief Set number of screen lines
	 * 
	 * @param lines Lines of text that fit on screen
	 * @return New value of screen lines
	 */
	void setScreenLines(int lines) {
		screenLines = lines;
	}

	/**
	 * @brief Scrolls text down a line. If text fits on screen, does nothing.
	 * 
	 */
	void scrollTextDown() {
		// This only fires if lines has been decremented already
		// It is thus redundant to check if scrollability is of concern.
		if (linesScrolled > 0) {
			linesScrolled--;
		}
	}
	/**
	 * @brief Scrolls text up a line. If text fits on screen, does nothing.
	 * 
	 * @param textLines Number of lines in text being scrolled
	 */
	void scrollTextUp(int textLines) {
		// Only allow scrolling if the current text does not fit on screen
		if (linesScrolled < textLines) {
			linesScrolled++;
		}
	}

	void resetScroll() {
		linesScrolled = 0;
	}
};
};

#endif