
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
#include <filesystem>

#include <cassert>
#include <cmath>


#include "video-sfml.h"
#include "video-sfml-compositor.h"


#include "compositor.h"

#include "structures.h"
#include "schedule.h"

#include "image.h"
#include "chapter.h"
#include "json-loader.h"

#include "debug.h"




#define GPU_RENDER 
/*
#ifdef GPU_RENDER
import AcceleratedText;
import AcceleratedRender;
#else 
import SoftwareText;
import SoftwareRender;
#endif

import SFMLRendering;
*/

using std::cout; //NOLINT(misc-unused-using-decls)
using std::cin;  //NOLINT(misc-unused-using-decls)
using std::endl; //NOLINT(misc-unused-using-decls)


using namespace vnpge;

typedef unsigned int uint;





int main() {
	
	return 0;
	
	SFMLCompositor c;
	CompositorArea a{ {.dimensions = {  .w = 0.5,
										.h = 0.5 },
						.position = {.srcPos =  {.x = 0.2,
											     .y = 0.2 },
									 .destPos = {.x = 0.3,
												 .y = 0.3 }
									}
					  },
	[]{return true; }, [](const auto& rt) {}};

	c.addArea("stella", "vnpge::foreground", a);
	c.addArea("flower", "stella", a);
	c.addArea("claire", "vnpge::foreground", a);
	c.addArea("pot hat", "claire", a);
	c.addArea("crown", "pot hat", a);

	c.computeOrder();

	
	//return 0;


	SFMLWindow window;
	
	
	TextBoxInfo boxInfo = { SDLInfo.getScreenDimensions(), {.w = 1.0, .h = 0.25} };
	
	#ifdef GPU_RENDER
	TextRenderer textRenderer = { SDLInfo.getWindowRenderer().getRenderer(), boxInfo, textBGGenerator, {"placeholder", "this is a bug", {0, 0, 0}}, {"assets/fonts/BonaNova-Italic.ttf"}};
	#else
	TextRenderer textRenderer = { SDLInfo.getScreenSurface(), boxInfo, textBGGenerator, {"placeholder", "this is a bug", {0, 0, 0}}, {"assets/fonts/BonaNova-Italic.ttf"}};
	#endif



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
					textRenderer.resetScroll();
				}
				break;

				case Action::prev_page: {
					chapter.prevFrame();
					textRenderer.resetScroll();
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
					#ifdef GPU_RENDER
					textRenderer.updateResolution(SDLInfo.getWindowRenderer().getRenderer(), info, textBGGenerator);
					#else
					textRenderer.updateResolution(SDLInfo.getScreenSurface(), info, textBGGenerator);
					#endif
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
