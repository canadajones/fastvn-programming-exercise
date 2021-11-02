#ifndef TEXTBOX_HEADER
#define TEXTBOX_HEADER

#include <SDL2/SDL_render.h>
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
#include "image.h"


namespace vnpge {

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

	std::shared_ptr<SDL_Renderer> renderer;
	std::shared_ptr<SDL_Texture> accelBox;
	
	

	SDL_Rect displayRect;

	// May change at an arbitrary time
	DialogueFont font;
	std::string fontName;
	
	// Regenerated every time the frame is changed
	std::shared_ptr<SDL_Surface> textSurface;

	
	std::shared_ptr<SDL_Texture> accelText;

	int lines;
	public:
	/**
	 * @brief Construct a new TextBox.
	 * 
	 * @param surfDimensions Dimensions of surface to which this box will be blitted later.
	 * @param font The font to use, in form of a DialogueFont.
	 * @param boxGenerator A function accepting the absolute dimensions of the destination surface, as well as the relative dimensions this box occupies thereon.
	 */
	TextBox(AbsoluteDimensions surfDimensions, std::string font, TextBoxCreator boxGenerator, SDL_Renderer* renderer) :
		
		box{boxGenerator(surfDimensions, relDimensions), SDL_FreeSurface},
		renderer{renderer, SDL_DestroyRenderer},
		accelBox{SDL_CreateTextureFromSurface(renderer, box.get()), SDL_DestroyTexture},
		displayRect{.x = 0, .y = 0, .w = box->w - 48, .h = box->h - 48},
		font{font, getPtSize(surfDimensions)}, fontName{font}, textSurface{nullptr},
		accelText{nullptr}, lines{0} {};


	const PositionMapping& getPosMap() const {
		return posMap;
	}
	SDL_Surface* getBox() const {
		return box.get();
	};
	
	SDL_Surface* getText() const {
		return textSurface.get();
	}

	SDL_Texture* getBoxAccel() const {
		return accelBox.get();
	};

	const SDL_Rect* getRect() const {
		return &displayRect;
	}
	
	SDL_Texture* getTextAccel() const {
		return accelText.get();
	}

	SDL_Surface* generateDisplayText(std::string text) {

		// TODO: make per-character text colouring a thing
		SDL_Color color = {255, 255, 255, 255};
		textSurface.reset(TTF_RenderUTF8_Blended_Wrapped(font.getFont(), text.c_str(), color, box->w - 48), SDL_FreeSurface);
		//SDL_SetSurfaceBlendMode(textSurface.get(), SDL_BLENDMODE_NONE);
		
		// Reset text position upon text change
		lines = 0;
		updateTextPosition();

		return textSurface.get();
	};

	AbsoluteDimensions updateResolution(AbsoluteDimensions surfDimensions, TextBoxCreator boxGenerator) {
		box.reset(boxGenerator(surfDimensions, relDimensions), SDL_FreeSurface);
		displayRect.w = box->w - 48;
		displayRect.h = box->h - 48;
		
		
		accelBox.reset(SDL_CreateTextureFromSurface(renderer.get(), box.get()), SDL_DestroyTexture);
		accelText.reset(SDL_CreateTextureFromSurface(renderer.get(), textSurface.get()), SDL_DestroyTexture);

		font = {fontName, getPtSize(surfDimensions)};
		return {static_cast<uint>(box->w), static_cast<uint>(box->h)};
	};

	void updateTextPosition() {
		int ptSize = getPtSize({.w = static_cast<uint>(box->w), .h = static_cast<uint>(box->h)});
		displayRect = {
			.x = 0,
			.y = lines * 2 * ptSize,
			.w = box->w - 48,
			.h = box->h - 48
		};
		accelText.reset(SDL_CreateTextureFromSurface(renderer.get(), textSurface.get()), SDL_DestroyTexture);
	};

	void scrollDown() {
		// This only fires if lines has been decremented already
		// It is thus redundant to check if scrollability is of concern.
		if (lines < 0) {
			lines++;
			updateTextPosition();
		}
	}
	void scrollUp() {
		// Only allow scrolling if the current text does not fit on screen
		// Also disallow further scrolling if the bottom of the text has been reached.
		// We check for this by seeing if the remainder of the text surface height post-scrolling is lesser than the display box height
		int pixelsMoved = lines * 2 * getPtSize({.w = static_cast<uint>(box->w), .h = static_cast<uint>(box->h)});
		
		if (textSurface->h > (box->h - 48 )&& (displayRect.y + pixelsMoved) < (box->h - 48)) {
			lines--;
			updateTextPosition();
		}
	}
};
};

#endif