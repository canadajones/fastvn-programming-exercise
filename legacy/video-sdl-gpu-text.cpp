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


export module AcceleratedText;

import StoryDialogue;


export namespace vnpge {

	class GPUFont : DialogueFont {
	private:
	std::shared_ptr<TTF_Font> font;

	public:
	GPUFont(DialogueFont& dfont, uint ptSize) : DialogueFont(dfont) {
		float hdpi, vdpi;
		SDL_GetDisplayDPI(0, nullptr, &hdpi, &vdpi);
		font = {TTF_OpenFontDPI(dfont.getName().c_str(), ptSize, hdpi, vdpi), TTF_CloseFont };

		if (font == nullptr) {
			std::string err = "Font could not be loaded. TTF_Error:";
			throw std::runtime_error(err.append(TTF_GetError()));
		}
	};

	GPUFont() = default;
	TTF_Font* getFont() {
		return font.get();
	}

	~GPUFont() {};
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


class FontStorage {
	private:
	std::unordered_map<std::string, vnpge::GPUFont> fontMap;
	static uint getPtSize(const AbsoluteDimensions& boxDims) {
		double boxHeight = static_cast<double>(boxDims.h) * 0.25;
		uint ptSize;
		uint numLines = 5;

		// TODO: Evaluate pixel values based on DPI

		while (boxHeight > 20) {
			ptSize = boxHeight / numLines;
			std::cout << "ptSize: " << ptSize << std::endl;
			if (ptSize > 30) {
				numLines += 1;
				continue;
			}
			if (ptSize < 10) {
				numLines -= 1;
				continue;
			}

			if (boxHeight / (numLines + 1) < 25) {
				std::cout << "boxheight: " << boxHeight << ", numLines: " << numLines << std::endl;
				break;
			}
			else {
				numLines += 1;
			}
		}
		numLines += static_cast<uint>(boxDims.h / boxDims.w);


		return static_cast<uint>(std::round(static_cast<double>(boxHeight) / static_cast<double>(numLines))) * 3;
	}
	public:
	GPUFont operator() (DialogueFont font, const AbsoluteDimensions& boxDims ) {
		if (!fontMap.contains(font.getName())) {
			fontMap.insert({font.getName(), {font, getPtSize(boxDims)}});
		}
		return fontMap.at(font.getName());
	}

	void clear() {
		fontMap.clear();
	}
};






/**
 * @brief Stateful renderer for textboxes
 * 
 */
class TextRenderer {
	private:

	// The destination surface
	SDL_Renderer* dest;
	

	std::shared_ptr<SDL_Texture> background;
	std::shared_ptr<SDL_Texture> text;

	int scrolledLines = 0;
	int lineHeight;

	FontStorage fontStorage;

	// Generated values, don't touch

	AbsoluteDimensions textArea;
	AbsolutePosition textPosition; // origin is at upper left of background
	
	public:
	TextRenderer(SDL_Renderer* dest, TextBoxInfo boxInfo, TextBGCreator<SDL_Surface*> bgCreator,
				 Dialogue dialogue, DialogueFont font) {
		updateResolution(dest, boxInfo, bgCreator);
		renderStoryFrame(dialogue, font);
	};
	
	SDL_Texture* renderStoryFrame(Dialogue dialogue, DialogueFont font) {

		// Grab dialogue colour
		SDL_Color fgcolour = {
			.r = static_cast<uint8_t>(dialogue.getColour().red),
			.g = static_cast<uint8_t>(dialogue.getColour().green),
			.b = static_cast<uint8_t>(dialogue.getColour().blue),
			.a = 255
		};

		// Look up font in font table
		GPUFont f = fontStorage(font, textArea);

		// Temporary storage for the complete text
		SDL_Surface* renderedText = TTF_RenderUTF8_Blended_Wrapped(f.getFont(), dialogue.getText().c_str(), fgcolour, textArea.w);
		
		// Store the updated text
		text.reset(SDL_CreateTextureFromSurface(dest, renderedText), SDL_DestroyTexture);


		// Free the temporary storage
		SDL_FreeSurface(renderedText);
		

		// Grab the line height of the text, for scrolling purposes
		lineHeight = TTF_FontLineSkip(f.getFont());

		return text.get();
	};

	void updateResolution(SDL_Renderer* newDest, TextBoxInfo boxInfo, TextBGCreator<SDL_Surface*> bgCreator) {
		dest = newDest;

		// Generate a new text background, complete with information about position and area available to actual text
		auto textBGSurface = bgCreator(boxInfo.getResolution(), boxInfo.getArea());
		
		background.reset(SDL_CreateTextureFromSurface(dest, textBGSurface.first), SDL_DestroyTexture);

		textArea = textBGSurface.second.area;
		textPosition = textBGSurface.second.position;
		
		SDL_FreeSurface(textBGSurface.first);

		fontStorage.clear();
		resetScroll();
	};

	void displayText(AbsolutePosition position) {
		
		int w, h;
		SDL_GetRendererOutputSize(dest, &w, &h);

		SDL_Rect destPos = {
			.x = position.x,
			.y = position.y,
			.w = w,
			.h = static_cast<int>(h * 0.25)
		};
		
		// Put box on screen (easy!)
		if (SDL_RenderCopy(dest, background.get(), nullptr, &destPos)) {
			throw std::runtime_error(SDL_GetError());
		}
		
		// Grab the text dimensions (reuses w and h from above; not neat, but the alternative is even less neat)
		// TODO: write a wrapper for QueryTexture and GetRendererOutputSize
		SDL_QueryTexture(text.get(), nullptr, nullptr, &w, &h);

		// Offset box pos by text position within the box
		destPos = {
			.x = position.x + textPosition.x,
			.y = position.y + textPosition.y,
			.w = w,
			.h = static_cast<int>((h > textArea.h) ? textArea.h : h)
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
		SDL_RenderCopy(dest, text.get(), &srcPos, &destPos);
	};

	/**
	 * @brief Scroll text up by a line. If scrolling would reveal no new text, do nothing.
	 * 
	 */
	void scrollTextUp() {
		int w, h;
		SDL_QueryTexture(text.get(), nullptr, nullptr, &w, &h);
		
		//	Is there more text than can fit  && is the area scrolled away < the difference between the displayable area and the text area 
		if (static_cast<int>(textArea.h) < h && scrolledLines * lineHeight < h - textArea.h) {
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

	void resetScroll() {
		scrolledLines = 0;
	}
};

};
