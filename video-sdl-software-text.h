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
#include "abstract_text.h"


namespace vnpge {

class DialogueFont {
	private:
		std::shared_ptr<TTF_Font> font;

	public:
		DialogueFont(std::string name, uint ptsize) : font{TTF_OpenFontIndex(name.c_str(), ptsize, 0), TTF_CloseFont} {
			if (font == nullptr) {
				std::string err = "Font could not be loaded: ";
				throw std::runtime_error(err.append(TTF_GetError()));
			}
		}
		const TTF_Font* getFont() const {
			return font.get();
		}
		TTF_Font* getFont() {
			return font.get();
		}
};

template <typename T>
using TextBoxCreator = std::function<T (AbsoluteDimensions, RelativeDimensions)>;

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

	TextBoxCreator<SDL_Surface*> boxGenerator;

	// Changes with resolution when resizing window
	std::shared_ptr<SDL_Surface> box;
	

	SDL_Rect displayRect;

	// May change at an arbitrary time
	DialogueFont dialogueFont;
	
	// Regenerated every time the frame is changed
	std::shared_ptr<SDL_Surface> textSurface;

	public:
	/**
	 * @brief Construct a new TextBox.
	 * 
	 * @param surfDimensions Dimensions of surface to which this box will be blitted later.
	 * @param font The font to use, in form of a DialogueFont.
	 * @param boxGenerator A function accepting the absolute dimensions of the destination surface, as well as the relative dimensions this box occupies thereon.
	 */
	TextBox(AbsoluteDimensions surfDimensions, TextBoxCreator<SDL_Surface*> boxGenerator) :
		
		boxGenerator{boxGenerator},
		box{boxGenerator(surfDimensions, relDimensions), SDL_FreeSurface},
		displayRect{.x = 0, .y = 0, .w = box->w - 48, .h = box->h - 48},
		textSurface{nullptr}, {};

	SDL_Surface* getBox() const {
		return box.get();
	};
	
	SDL_Surface* getText() const {
		return textSurface.get();
	}

	const SDL_Rect* getRect() const {
		return &displayRect;
	}
	

	SDL_Surface* generateDisplayText(std::string text) {

		// TODO: make per-character text colouring a thing
		SDL_Color color = {255, 255, 255, 255};
		textSurface.reset(TTF_RenderUTF8_Blended_Wrapped(dialogueFont.getFont(), text.c_str(), color, box->w - 48), SDL_FreeSurface);
		//SDL_SetSurfaceBlendMode(textSurface.get(), SDL_BLENDMODE_NONE);
		
		// Reset text position upon text change
		updateTextPosition(0);

		return textSurface.get();
	};

	AbsoluteDimensions updateResolution(AbsoluteDimensions newDimensions, RelativeDimensions position, DialogueFont& font) {
		// Generate new text box background
		box.reset(boxGenerator(newDimensions, position), SDL_FreeSurface);
		
		// Update text display rect area
		displayRect.w = box->w - 48;
		displayRect.h = box->h - 48;
		
		// Update font size
		dialogueFont = font;
		
		return {static_cast<uint>(box->w), static_cast<uint>(box->h)};
	};

	void updateTextPosition(int lines) {
		int ptSize = getPtSize({.w = static_cast<uint>(box->w), .h = static_cast<uint>(box->h)});
		displayRect = {
			.x = 0,
			.y = lines * 2 * ptSize,
			.w = box->w - 48,
			.h = box->h - 48
		};
		
	};
};

class TextBoxDisplayable {
	private:
	TextBoxInfo textBoxInfo;
	DialogueFont font;
	TextBox renderingTextBox;
	public:

	void updateResolution() {

	}


};
};

// Make a Dialogue class?
#endif