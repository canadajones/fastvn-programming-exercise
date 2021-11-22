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
namespace sw {

class Dialogue {
	private:
	std::string name;
	std::string text;
	Colour colour;
	public:

	Dialogue(std::string name, std::string text, Colour colour) : name{name}, text{text}, colour{colour} {};

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
	RelativeDimensions relativeArea;

	public:
	TextBoxInfo(RelativeDimensions area) : relativeArea{area} {};

	RelativeDimensions getArea() {
		return relativeArea;
	};
};

// FONTS!!!!

/**
 * @brief Stateful renderer for textboxes
 * 
 */
class TextRenderer {
	private:
		std::shared_ptr<SDL_Surface> background;
		std::shared_ptr<SDL_Surface> text;

		int scrolledLines;

		// Generated values, don't touch

		AbsoluteDimensions textArea;
		AbsolutePosition textPosition; // origin is at upper left of background

	public:

	TextRenderer() {};
	SDL_Surface* renderStoryFrame(Dialogue dialogue) {
		return nullptr;
	};

	void updateResolution(AbsoluteDimensions resolution, TextBoxInfo boxInfo, TextBGCreator<SDL_Surface*> bgCreator) {
		
		// Generate a new text background, complete with information about position and area available to actual text
		auto textBGSurface = bgCreator(resolution, boxInfo.getArea());

		background.reset(textBGSurface.first, SDL_FreeSurface);

		textArea = textBGSurface.second.area;


	};
};

};
};

#endif