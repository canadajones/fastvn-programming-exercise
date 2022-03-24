
#define SDL_MAIN_HANDLED

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <iterator>
#include <memory>
#include <unordered_map>

#include <cassert>
#include <cmath>

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include "video-sdl-common.h"

#include "structures.h"
#include "schedule.h"

#include "debug.h"


import Image;
import Chapter;
import SoftwareText;
import JSONLoader;
import SoftwareRender;

using std::cout; //NOLINT(misc-unused-using-decls)
using std::cin;  //NOLINT(misc-unused-using-decls)
using std::endl; //NOLINT(misc-unused-using-decls)


using namespace vnpge;

typedef unsigned int uint;



std::pair<SDL_Surface*, PositionedArea> textBGGenerator(AbsoluteDimensions screenSize, RelativeDimensions bgArea) {
	const int borderThickness = 4;
	const int padding = 10;
	
	// Default simple text box
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		SDL_Surface* textBoxSurface = SDL_CreateRGBSurface(0, screenSize.w * bgArea.w, screenSize.h * bgArea.h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	#else
		SDL_Surface* textBoxSurface = SDL_CreateRGBSurface(0, screenSize.w * bgArea.w, screenSize.h * bgArea.h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	#endif

	if (textBoxSurface == nullptr) {
		std::string err = "Surface could not be created! SDL_Error: ";
		throw std::runtime_error(err.append(SDL_GetError()));
	}

	SDL_Rect textBox = {
		.x = 0,
		.y = 0,
		.w = textBoxSurface->w,
		.h = textBoxSurface->h
	};

	// Fill with a transparent white
	SDL_FillRect(textBoxSurface, &textBox, SDL_MapRGBA(textBoxSurface->format, 0xFF, 0xFF, 0xFF, 0x7F));

	
	textBox.x += borderThickness;
	textBox.y += borderThickness;
	textBox.w -= borderThickness * 2;
	textBox.h -= borderThickness * 2;

	// Fill with a transparent black inside the other rectangle, thereby creating a white border
	// Note that FillRect does not blend alphas, so this alpha replaces the white's
	// This is actually desirable, since the colours should have different alphas in order to feel equally transparent
	SDL_FillRect(textBoxSurface, &textBox, SDL_MapRGBA(textBoxSurface->format, 0x30, 0x30, 0x30, 0xAF));

	return {
		textBoxSurface, {
			.area		= { 
							.w = static_cast<uint>(textBox.w) - 2 * padding,
							.h = static_cast<uint>(textBox.h) - 2 * padding},
			.position =	  {	.x = borderThickness + padding, 
							.y = borderThickness + padding}
		}
	};
}


int main() {
	SWRenderManager SDLInfo{};
	TextBoxInfo boxInfo = { SDLInfo.getScreenDimensions(), {.w = 1.0, .h = 0.25} };
	TextRenderer textRenderer = { SDLInfo.getScreenSurface(), boxInfo, textBGGenerator, {"placeholder", "this is a bug", {0, 0, 0}}, {"assets/fonts/BonaNova-Italic.ttf"} };


	// Load chapter data
	JSONLoader loader = {"assets/scripts/index.json"};
	Chapter chapter = loader.loadChapter();


	auto& curFrame = chapter.curFrame;

	
	renderFrame(SDLInfo, *curFrame, textRenderer);
	
	
	while (true)   {
		auto events = handleEvents();
		for (auto& ev : events) {
			switch (ev.getAction()) {
				
				case Action::clean_exit: {
					return 0;
				}
				break;

				case Action::next_page: {
					chapter.nextFrame();
				}
				break;

				case Action::prev_page: {
					chapter.prevFrame();
				}
				break;

				case Action::scroll_up: {
					textRenderer.scrollTextUp();
				}
				break;

				case Action::scroll_down: {
					textRenderer.scrollTextDown();
				}
				break;

				case Action::window_resized: {
					// This may look complicated, but all it does is create a AbsoluteDimensions object containing the new resolution
					// chapter.updateResolution({static_cast<uint>(ev.getData().first), static_cast<uint>(ev.getData().second)}, makeTextBox);
					AbsoluteDimensions screenDims = {static_cast<uint>(ev.getData().first), static_cast<uint>(ev.getData().second) };
					TextBoxInfo info = { screenDims, {.w = 1.0, .h = 0.25} };
					textRenderer.updateResolution(SDLInfo.getScreenSurface(), info, textBGGenerator);
				}
				break;
				case Action::nothing: {
					cout << "attention! an event was issued to do literally nothing. this is a bug, please report it." << endl;
				}
				break;
			}
			
			// If any of the previous events caused us to get to the end of the chapter, exit cleanly before we encounter an error.
			// The error in question would be invalid dereferencing of the end iterator.
			if (curFrame == chapter.storyFrames.end()) {
				return 0;
			}
			
			
			// Only render the frame if there is anything to do.
			// All current events cause a screen change, so reaching this point means it has to be run.
			
			renderFrame(SDLInfo, *curFrame, textRenderer);
		}
		
		SDL_Delay(10);
	}

	// TODO:

	// "System" queues, to enable more flexibility in terms of input and effects and such
	// Add "dynamic" loading of code (adding new modules may require a recompile, but simply including it should be enough to load it)
	// Sound
	
	return 0;
}
