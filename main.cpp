
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

#include "video-sdl-common.h"
#include "video-sdl-gpu.h"
//#include "video-sdl-software.h"

#include "structures.h"
#include "chapter.h"
#include "image.h"
#include "text.h"
#include "schedule.h"


// DEPRECATED
//#include "data.h"

#include "jsonloader.h"

using std::cout; //NOLINT(misc-unused-using-decls)
using std::cin;  //NOLINT(misc-unused-using-decls)
using std::endl; //NOLINT(misc-unused-using-decls)


using namespace vnpge;

typedef unsigned int uint;




int main() {
	gpu::SDLManager SDLInfo;
	
	//sw::SDLManager SDLInfo;
	//SDL_Surface* screenSurface = SDLInfo.getScreenSurface();

	// DEPRECATED
	// Chapter test = initTest(static_cast<uint>(screenSurface->w), static_cast<uint>(screenSurface->h));
	
	// Load chapter data
	Chapter chapter = JSONLoader{"index.json"}.loadChapter(SDLInfo.getScreenDimensions(), SDLInfo.getRenderer());

	auto& curFrame = chapter.curFrame;
	
	TexContainer texCon;
	texCon.bg.reset(SDL_CreateTextureFromSurface(SDLInfo.getRenderer(), curFrame->bg.getSurface()), SDL_DestroyTexture);
	texCon.character.reset(SDL_CreateTextureFromSurface(SDLInfo.getRenderer(), curFrame->storyCharacter.expressions.at(curFrame->expression).getSurface()), SDL_DestroyTexture);
	renderFrameAccel(SDLInfo, *curFrame, texCon, chapter.textBox);
	
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
					chapter.textBox.scrollTextUp();
				}
				break;

				case Action::scroll_down: {
					chapter.textBox.scrollTextDown();
				}
				break;

				case Action::window_resized: {
					// This may look complicated, but all it does is create a AbsoluteDimensions object containing the new resolution
					chapter.updateResolution({static_cast<uint>(ev.getData().first), static_cast<uint>(ev.getData().second)}, makeTextBox);
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
			texCon.bg.reset(SDL_CreateTextureFromSurface(SDLInfo.getRenderer(), curFrame->bg.getSurface()), SDL_DestroyTexture);
					texCon.character.reset(SDL_CreateTextureFromSurface(SDLInfo.getRenderer(), curFrame->storyCharacter.expressions.at(curFrame->expression).getSurface()), SDL_DestroyTexture);
			
			// Only render the frame if there is anything to do.
			// All current events cause a screen change, so reaching this point means it has to be run.
			renderFrameAccel(SDLInfo, *curFrame, texCon, chapter.textBox);
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
