module;
#include <SDL2/SDL_render.h>
#include <limits>
#include <memory>
#include <stdexcept>
#include <functional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_pixels.h>

#include <SDL2/SDL_image.h>

#include <SDL2/SDL_ttf.h>

#include "video-sdl-common.h"

#include "structures.h"
#include "abstract_text.h"

export module SoftwareText;

import StoryDialogue;

namespace vnpge {
namespace sw {

// FONTS!!!!

class DialogueFont {
	private:
	std::string name;
	
	public:	

	// TODO: put in getPtSize here from the other file
	DialogueFont(std::string path) : name{path} {};

	std::string getName() {
		return name;
	}
};

class SWFont : DialogueFont {
	private:
	std::shared_ptr<TTF_Font> font;
	
	public:
	SWFont(DialogueFont& font) : DialogueFont(font), font{TTF_OpenFont(font.getName().c_str(), 20), TTF_CloseFont} {};

	TTF_Font* getFont() {
		return font.get();
	}
};

class Dialogue {
	private:
	std::string name;
	std::string text;
	Colour colour;
	DialogueFont font;

	public:
	Dialogue(std::string name, std::string text, Colour colour, DialogueFont& font) :
		name{name}, text{text}, colour{colour}, font{font} {};

	std::string getName() const {
		return name;
	}

	std::string getText() const {
		return text;
	}

	Colour getColour() const {
		return colour;
	}
};



//														  Usable text area	   Screen size	   Text box area relative to screen size
template<typename Renderable>
using TextBGCreator = std::function<std::pair<Renderable, PositionedArea> (AbsoluteDimensions, RelativeDimensions)>;

class TextBoxInfo {
	private:
	AbsoluteDimensions resolution;
	RelativeDimensions relativeArea;

	public:
	TextBoxInfo(AbsoluteDimensions resolution, RelativeDimensions area) : resolution{resolution}, relativeArea{area} {};

	AbsoluteDimensions getResolution() {
		return resolution;
	};

	RelativeDimensions getArea() {
		return relativeArea;
	};
};

SWFont lookupFont(DialogueFont font) {
	// TODO: Make this look the font up in a hashmap 
	return {font};
}


/**
 * @brief Stateful renderer for textboxes
 * 
 */
class TextRenderer {
	private:
	// The destination surface
	SDL_Surface* dest;
	
	std::shared_ptr<SDL_Surface> background;
	std::shared_ptr<SDL_Surface> text;

	int scrolledLines;
	int lineHeight;

	// Generated values, don't touch

	AbsoluteDimensions textArea;
	AbsolutePosition textPosition; // origin is at upper left of background
	
	public:
	TextRenderer(SDL_Surface* dest, TextBoxInfo boxInfo, TextBGCreator<SDL_Surface*> bgCreator,
				 Dialogue dialogue, DialogueFont font) : dest{dest} {
		updateResolution(boxInfo, bgCreator);
		renderStoryFrame(dialogue, font);
	};

	
	SDL_Surface* renderStoryFrame(Dialogue dialogue, DialogueFont font) {

		// Grab dialogue colour
		SDL_Color fgcolour = {
			.r = static_cast<uint8_t>(dialogue.getColour().red),
			.g = static_cast<uint8_t>(dialogue.getColour().green),
			.b = static_cast<uint8_t>(dialogue.getColour().blue),
			.a = 255
		};

		// Look up font in font table
		SWFont f = lookupFont(font);

		// Temporary storage for the complete text
		SDL_Surface* renderedText = TTF_RenderUTF8_Blended_Wrapped(f.getFont(), dialogue.getText().c_str(), fgcolour, textArea.w);
		
		text.reset(renderedText, SDL_FreeSurface);

		lineHeight = TTF_FontLineSkip(f.getFont());

		return text.get();
	};

	void updateResolution(TextBoxInfo boxInfo, TextBGCreator<SDL_Surface*> bgCreator) {
		
		// Generate a new text background, complete with information about position and area available to actual text
		auto textBGSurface = bgCreator(boxInfo.getResolution(), boxInfo.getArea());

		background.reset(textBGSurface.first, SDL_FreeSurface);

		textArea = textBGSurface.second.area;
		textPosition = textBGSurface.second.position;
	};

	void displayText(AbsolutePosition position) {
		
		SDL_Rect destPos = {
			.x = position.x,
			.y = position.y,
		};
		
		// Put box on screen (easy!)
		SDL_BlitSurface(background.get(), nullptr, dest, &destPos);

		// Offset box pos by text position within the box
		destPos = {
			.x = position.x + textPosition.x,
			.y = position.y + textPosition.y
		};

		// Pick a rectangle representing the current scrolled level
		// x is always 0
		// y is text scroll
		// w and h are given by the box generator function as the writable part of the box
		SDL_Rect srcPos = {
			.x = 0,
			.y = scrolledLines * lineHeight,
			.w = static_cast<int>(textArea.w),
			.h = static_cast<int>(textArea.h)
		};
		
		// Put text on screen (less easy!)
		SDL_BlitSurface(text.get(), &srcPos, dest, &destPos);
	};

	/**
	 * @brief Scroll text up by a line. If scrolling would reveal no new text, do nothing.
	 * 
	 */
	void scrollTextUp() {
		
		if (static_cast<int>(textArea.h) < text->h && scrolledLines * lineHeight <= text->h) {
			scrolledLines++;
		}
	};
	/**
	 * @brief Scroll text down by a line. If scrolling would reveal no new text, do nothing.
	 * 
	 */
	void scrollTextDown() {
		if (scrolledLines > 0) {
			scrolledLines--;
		}
	}

};

};
};
