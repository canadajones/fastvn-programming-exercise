#include <memory>
#include <vector>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

#include <SDL2/SDL_image.h>

#include <SDL2/SDL_ttf.h>

#include "structures.h"
#include "chapter.h"
#include "image.h"
#include "text.h"
#include "schedule.h"

using namespace vnpge;

class SDLManager {
	private:
	SDL_Window* window;
	std::shared_ptr<SDL_Renderer> renderer;
	
	public:
	SDLManager() {
		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
			std::string err = "SDL could not initialize! SDL_Error: ";
			throw std::runtime_error(err.append(SDL_GetError()));
		} 	

		// Load support for the JPG and PNG image formats
		int flags = IMG_INIT_JPG | IMG_INIT_PNG;
		if (IMG_Init(flags) ^ flags) {
			std::string err = "SDL_Image could not initialize! IMG_Error: ";
			throw std::runtime_error(err.append(IMG_GetError()));
		}

		// Grab font rendering library
		if (TTF_Init()) {
			std::string err  = "TTF_Init: "; 
			throw std::runtime_error(err.append(TTF_GetError()));
		}


		//Create window
		window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 800, SDL_WINDOW_FULLSCREEN_DESKTOP);

		//window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 600, SDL_WINDOW_RESIZABLE);

		if (window == nullptr) {
			std::string err = "Window could not be created! SDL_Error: "; 
			throw std::runtime_error(err.append(SDL_GetError()));
		}

		// Initialise renderer as a GPU-accelerated one, that renders at vsynch, and that is destroyed by the proper SDL function
		renderer.reset(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC), SDL_DestroyRenderer);
	}

	SDLManager(const SDLManager&) = delete;
	SDLManager operator=(const SDLManager&) = delete;

	~SDLManager() {
		// Unload font support
		TTF_Quit();

		// Unload image format loading support
		IMG_Quit();
		
		// Destroy window
		SDL_DestroyWindow(window);
		
		// Quit SDL subsystems
		SDL_Quit();
	}


	SDL_Window* getWindow() {
		return window;
	}

	SDL_Renderer* getRenderer() {
		return renderer.get();
	}
	std::shared_ptr<SDL_Renderer> getRendererShared() {
		return renderer;
	}
};


/**
 * @brief Renders a SDL_Texture onto a SDL_Renderer
 * 
 * @param renderer The SDL renderer to use.
 * @param src Source texture.
 * @param posMap Relative position mapping between source and destination. 
 * @param scale_percentage Percentage points to scale the source by before rendering.
 * @return The return status code of the underlying SDL_RenderCopy function. 
 */
int renderTextureConstAspectRatio(SDL_Renderer* renderer, SDL_Texture* src, PositionMapping posMap, uint scale_percentage = 100) {
	
	// The SDL gods demand a position sacrifice
	SDL_Rect pos;
	
	// Convenience variables
	int srcTextureWidth;
	int srcTextureHeight;
	int destWidth;
	int destHeight;

	SDL_QueryTexture(src, nullptr, nullptr, &srcTextureWidth, &srcTextureHeight);
	SDL_GetRendererOutputSize(renderer, &destWidth, &destHeight);

	// Create both ratios now, to enable easier comparison with the later src ratios
	double destWidthDivHeightRatio = static_cast<double>(destWidth) / static_cast<double>(destHeight);
	double destHeightDivWidthRatio = static_cast<double>(destHeight) / static_cast<double>(destWidth);

	// It's easier to define both the width/height and height/width ratios than it is to compute only one of the two, and then
	// take the reciprocal of it. Besides, in cases of surfaces with extreme ratios, even though we're using doubles, one might end up with
	// one of these being inaccurate. Note that this is purely theoretical, and that I reckon this will be fine for all practical purposes.
	// Still neater code-wise though, so I'm keeping this for the foreseeable future.
	double srcWidthDivHeightRatio = static_cast<double>(srcTextureWidth) / static_cast<double>(srcTextureHeight);
	double srcHeightDivWidthRatio = static_cast<double>(srcTextureHeight) / static_cast<double>(srcTextureWidth);
	
	// Reduce the integer percentage to a double we can multiply into other expressions
	double scale = static_cast<double>(scale_percentage) / static_cast<double>(100.0);

	// This is a tricky bit of application-specific code
	// Essentially, if the aspect ratio of the src and dest mismatch, different space-filling methods must be used
	// If srcW/srcH < destW/destH, or in other words, the source is less wide than the destination, it will fill the dest tall-wise
	// In the opposite case, the source is wider, and it will fill up the width as much as possible
	// If they are of equal aspect ratio, the method used is irrelevant, and either will work
	// Note that the 'or' here is technically redundant. Either both checks will succeed, or both will fail.
	// In cases of (hypothetical) float distortion, one of these checks might (hypothetically) fail, but both shouldn't
	// Therefore there is an or here, though it is highly unlikely it would come to use.
	if (srcWidthDivHeightRatio < destWidthDivHeightRatio || srcHeightDivWidthRatio > destHeightDivWidthRatio) {
		pos.h = static_cast<uint>(std::round(scale * destHeight));
		pos.w = static_cast<uint>(std::round(scale * destHeight * srcWidthDivHeightRatio));
	}
	else {
		pos.h = static_cast<uint>(std::round(scale * destWidth * srcHeightDivWidthRatio));
		pos.w = static_cast<uint>(std::round(scale * destWidth));
	}


	AbsoluteDimensions srcDim = {
		.w = static_cast<uint>(pos.w),
		.h = static_cast<uint>(pos.h)
	};

	AbsoluteDimensions destDim = {
		.w = static_cast<uint>(destWidth),
		.h = static_cast<uint>(destHeight)
	};
	// Get x and y coordinates of the screen to be blitted
	AbsolutePosition xy = getPixelPosfromPosition(srcDim, destDim, posMap);

	pos.x = xy.x;
	pos.y = xy.y;


	return SDL_RenderCopy(renderer, src, nullptr, &pos);
};


class TexContainer {
	public:
	std::shared_ptr<SDL_Texture> bg;
	std::shared_ptr<SDL_Texture> character;
};

void renderTextAccel(SDL_Renderer* renderer, TextBox& textBox, std::string text) {

	PositionMapping posMap = textBox.getPosMap();
	posMap = {
		.srcPos = posMap.srcPos,
		.destPos = posMap.destPos
	};

	AbsoluteDimensions srcDim = {
		.w = static_cast<uint>(textBox.getBox()->w),
		.h = static_cast<uint>(textBox.getBox()->h),
	};

	int x;
	int y;
	SDL_GetRendererOutputSize(renderer, &x, &y);
	
	AbsoluteDimensions destDim = {
		.w = static_cast<uint>(x),
		.h = static_cast<uint>(y)
	};
	
	AbsolutePosition xy = getPixelPosfromPosition(srcDim, destDim, posMap);

	SDL_Rect pos = {
		.x = xy.x,
		.y = xy.y,
		.w = textBox.getBox()->w,
		.h = textBox.getBox()->h,
	};

	//SDL_BlitSurface(textBox.getBox(), nullptr, screenSurface, &pos);
	SDL_RenderCopy(renderer, textBox.getBoxAccel(), nullptr, &pos);

	// TODO: make this relative to the screen
	pos.x += 20;
	pos.y += 20;

	//SDL_BlitSurface(textBox.getText(), nullptr, screenSurface, &pos);
	SDL_RenderCopy(renderer, textBox.getTextAccel(), nullptr, &pos);
};

void renderFrameAccel(SDLManager& SDLInfo, Frame& curFrame, TexContainer& texCon, TextBox textBox) {
	/* Render loop:
	   	
		Background - x
			|- Transforms
			|- Transitions
		Characters
			|- Position - x
			|- Multiple in a frame
			|- Expressions - x
			|		|- Expression builder?
			|- Animations?
			|- Transforms
			|- Transitions
		Dialogue
			|- Text box - x
			|- Text - x
			|- Autofitting - x?
			|- Custom design?
		Graphical interface
			|- Keymaps - x?
			|- Menu
			|- Mouse clickies
			
	*/

	// Initial setup
	SDL_Renderer* renderer = SDLInfo.getRenderer();
	//SDL_Window* window = SDLInfo.getWindow();
	Image screen = SDLInfo.getScreenImage();

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	// Background
	
	SDL_RenderClear(renderer);
	
	//SDL_FillRect(screen.getSurface(), nullptr, SDL_MapRGB(screen.getSurface()->format, 255, 255, 255));
	PositionMapping posMap = {
		.srcPos = {0.5, 0.5},
		.destPos = {0.5, 0.5},
	};
	
	renderTextureConstAspectRatio(renderer, texCon.bg.get(), posMap, 100);

	// Characters
	
	// TODO: custom expression handlers
	//blitTextureConstAspectRatio(curFrame.storyCharacter.expressions.at(curFrame.expression), screen, curFrame.position, 80);
	renderTextureConstAspectRatio(renderer, texCon.character.get(), curFrame.position, 80);
	

	// Text
	renderTextAccel(renderer, textBox, curFrame.textDialogue);
	
	SDL_RenderPresent(renderer);
	//SDL_UpdateWindowSurface(window);

}
Schedule<Event> handleEvents() {
	
	std::vector<Event> events;

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN: {
				switch(event.key.keysym.sym) {
					case SDLK_ESCAPE: {
						// 137; magic number for program exit
						events.emplace_back(Action::clean_exit);
					}
					break;
					case SDLK_RIGHT:
					case SDLK_c:
					case SDLK_SPACE: {
						if (!event.key.repeat) {
							events.emplace_back(Action::next_page);
						}
					}
					break;
					case SDLK_LEFT:
					case SDLK_z: {
						if (!event.key.repeat) {
							events.emplace_back(Action::prev_page);
						}
					}
					break;

					case SDLK_UP:{
						if (!event.key.repeat) {
							events.emplace_back(Action::scroll_up);
						}
					}
					break;
					case SDLK_DOWN:{
						if (!event.key.repeat) {
							events.emplace_back(Action::scroll_down);
						}
					}
					break;
				}
			}
			break;
			case SDL_WINDOWEVENT: {
				switch(event.window.event) {
					case SDL_WINDOWEVENT_RESIZED: {
						events.emplace_back(Action::window_resized, event.window.data1, event.window.data2);
					}
					break;
					case SDL_WINDOWEVENT_CLOSE: {
						events.emplace_back(Action::clean_exit);
					}
				}
			}
			break;
		}
	}
	Schedule<Event> evSched = {events};
	return evSched;
}
