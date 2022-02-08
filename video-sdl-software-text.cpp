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


export module SoftwareText;

import StoryDialogue;


namespace vnpge {
	uint getPtSize(AbsoluteDimensions boxDims) {
		double boxHeight = static_cast<double>(boxDims.h) * 0.25;
		uint ptSize;
		uint numLines = 5;

		// TODO: Evaluate pixel values based on DPI

		while (boxHeight > 40) {
			ptSize = boxHeight / numLines;
			if (ptSize > 60) {
				numLines += 1;
				continue;
			}
			if (ptSize < 15) {
				numLines -= 1;
				continue;
			}

			if (boxHeight / (numLines + 1) < 35) {
				break;
			}
			else {
				numLines += 1;
			}
		}
		numLines += static_cast<uint>(boxDims.h / boxDims.w);
		return static_cast<uint>(std::round(static_cast<double>(boxHeight) / static_cast<double>(numLines))) * 4;
	}
}

export namespace vnpge {
	class SWFont : DialogueFont {
	private:
		std::shared_ptr<TTF_Font> font;
	public:
		SWFont(DialogueFont& dfont, uint ptSize) : DialogueFont(dfont) {
			float hdpi, vdpi;
			SDL_GetDisplayDPI(0, nullptr, &hdpi, &vdpi);
			font = {TTF_OpenFontDPI(dfont.getName().c_str(), ptSize, hdpi, vdpi), TTF_CloseFont };
		};

		SWFont() = default;
		TTF_Font* getFont() {
			return font.get();
		}

		~SWFont() {};
	};

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



SWFont lookupFont(DialogueFont font, AbsoluteDimensions& boxDims, std::unordered_map<std::string, vnpge::SWFont>& fontMap) {
	// TODO: Make this look the font up in a hashmap 

	if (fontMap.contains(font.getName())) {
		return fontMap.at(font.getName());
	}
	fontMap.insert({font.getName(), {font, getPtSize(boxDims)}});
	return fontMap.at(font.getName());
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

	int scrolledLines = 0;
	int lineHeight;

	// Generated values, don't touch

	AbsoluteDimensions textArea;
	AbsolutePosition textPosition; // origin is at upper left of background
	
	std::unordered_map<std::string, vnpge::SWFont> fontMap;
	public:
	TextRenderer(SDL_Surface* dest, TextBoxInfo boxInfo, TextBGCreator<SDL_Surface*> bgCreator,
				 Dialogue dialogue, DialogueFont font) : dest{dest} {
		updateResolution(boxInfo, bgCreator);
		renderStoryFrame(dialogue, font);
	};
	TextRenderer() = default;
	
	SDL_Surface* renderStoryFrame(Dialogue dialogue, DialogueFont font) {

		// Grab dialogue colour
		SDL_Color fgcolour = {
			.r = static_cast<uint8_t>(dialogue.getColour().red),
			.g = static_cast<uint8_t>(dialogue.getColour().green),
			.b = static_cast<uint8_t>(dialogue.getColour().blue),
			.a = 255
		};

		// Look up font in font table
		SWFont f = lookupFont(font, textArea, fontMap);

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
