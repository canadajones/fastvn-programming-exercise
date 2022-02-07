
#define SDL_MAIN_HANDLED

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <iterator>
#include <memory>

#include <cassert>
#include <cmath>

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include "video-sdl-common.h"

#include "structures.h"
#include "schedule.h"



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



std::pair<SDL_Surface*, PositionedArea> makeTextBG(AbsoluteDimensions screenDims, RelativeDimensions boxDims) {
	Uint32 rmask, gmask, bmask, amask;
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
	#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
	#endif
	
	int boxW = screenDims.w * boxDims.w;
	int boxH = screenDims.h * boxDims.h;

	SDL_Surface* textSurf = SDL_CreateRGBSurface(0, boxW, boxH, 32, rmask, gmask, bmask, amask);
	SDL_Rect rect = {
		.x = 0,
		.y = 0,
		.w = boxW,
		.h = boxH
	};
	SDL_FillRect(textSurf, &rect, SDL_MapRGBA(textSurf->format, 0, 0, 0, 100));

	
	rect = {
		.x = 4,
		.y = 4,
		.w = boxW - 8,
		.h = boxH - 8
	};

	SDL_FillRect(textSurf, &rect, SDL_MapRGBA(textSurf->format, 255, 255, 255, 200));

	PositionedArea area = {
		.area = {
			.w = static_cast<uint>(boxW - 8),
			.h = static_cast<uint>(boxH - 8)
		},
		.position = {
			.x = 4,
			.y = 4
		}
	};

	return {textSurf, area};
};

int main() {
	SWRenderManager SDLInfo{};
	TextRenderer textRenderer;
	// Load chapter data
	JSONLoader loader = {"./index.json"};
	Chapter chapter = loader.loadChapter(SDLInfo.getScreenDimensions());

	// Move text renderer out of static storage and into main-local variable
	// This could potentially fix some font errors, and make making new text boxes easier

	initRenderer(SDLInfo, { "testPerson", "test", {.red=0, .green=0, .blue=0} }, { "BonaNova-Italic.ttf" });

	auto& curFrame = chapter.curFrame;
	
	
	
	while (true) {
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
					scrollTextUp();
				}
				break;

				case Action::scroll_down: {
					scrollTextDown();
				}
				break;

				case Action::window_resized: {
					// This may look complicated, but all it does is create a AbsoluteDimensions object containing the new resolution
					// chapter.updateResolution({static_cast<uint>(ev.getData().first), static_cast<uint>(ev.getData().second)}, makeTextBox);
					AbsoluteDimensions screenDims = {static_cast<uint>(ev.getData().first), static_cast<uint>(ev.getData().second) };
					TextBoxInfo info = { screenDims, {0.25, 1.0} };
					updateResolution(info, makeTextBG);
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
			renderFrame(SDLInfo, *curFrame);
		}
		
		SDL_Delay(10);
	}

	// TODO:
	// Make a loader at some point, though after the main displayer works

	// "System" queues, to enable more flexibility in terms of input and effects and such
	// Add "dynamic" loading of code (adding new modules may require a recompile, but simply including it should be enough to load it)
	// Sound
	
	return 0;
}
