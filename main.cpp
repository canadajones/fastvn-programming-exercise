#define SDL_MAIN_HANDLED

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <functional>
#include <iterator>
#include <memory>

#include <cassert>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

#include <SDL2/SDL_image.h>

#include <SDL2/SDL_ttf.h>

#include "structures.h"
#include "data.h"


using std::cout; //NOLINT(misc-unused-using-decls)
using std::cin;  //NOLINT(misc-unused-using-decls)
using std::endl; //NOLINT(misc-unused-using-decls)


using namespace vnpge;

typedef unsigned int uint;



class SDLManager {
	private:
	SDL_Window* window;
	SDL_Surface* screenSurface;
	Image screen;
	
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
			
		}

		// Grab font rendering library
		if (TTF_Init()) {
			std::string err  = "TTF_Init: "; 
			throw std::runtime_error(err.append(TTF_GetError()));
		}


		//Create window
		window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 800, SDL_WINDOW_FULLSCREEN_DESKTOP);

		if (window == nullptr) {
			std::string err = "Window could not be created! SDL_Error: "; 
			throw std::runtime_error(err.append(SDL_GetError()));
		}
		
		//Get window surface
		screenSurface = SDL_GetWindowSurface(window);


		screen = Image(screenSurface, true);
	}

	SDLManager(const SDLManager&) = delete;
	SDLManager operator=(const SDLManager&) = delete;

	~SDLManager() {
		
		// Unload font support
		TTF_Quit();

		// Unload image format loading support
		IMG_Quit();
		
		//Destroy window
		SDL_DestroyWindow(window);
		
		//Quit SDL subsystems
		SDL_Quit();
	}


	SDL_Surface* getScreenSurface() {
		return screenSurface;
	}
	SDL_Window* getWindow() {
		return window;
	}

	Image& getScreenImage() {
		return screen;
	}
};




AbsolutePosition getPixelPosfromPosition(PositionMapping& posMap) {

	// It contains a pair of points, plus the widths and heights of the two surfaces
	// The points are in the form of normalised doubles, where 0,0 is the top left corner,
	// and 1,1 is the bottom right corner.
	// Each has been normalised according to its own surface's width and height, so we first have to unpack this into pixels
	// We need the pixels because we need to have a unit of distance which is the same distance in both coordinate systems
	// Simply using the doubles would give an incorrect result, as it would be equivalent to dest_x/dest/width - src_x/src_width,
	// which I hope is obvious wouldn't work. You can't just add and subtract percentages!

	int srcLocalPixPosX = posMap.srcWidth * posMap.srcPos.x;

	int srcLocalPixPosY = posMap.srcHeight * posMap.srcPos.y;

	int destLocalPixPosX = posMap.destWidth * posMap.destPos.x;

	int destLocalPixPosY = posMap.destHeight * posMap.destPos.y;

	// Now that we have the variables unpacked, use them
	// Technically, the src coordinates here are being used for their distance from the origin
	// But since the taxicab distance and numerical value of the point is the same, it works just fine

	int srcDestPixPosX = static_cast<uint>(destLocalPixPosX - srcLocalPixPosX);
	int srcDestPixPosY = static_cast<uint>(destLocalPixPosY - srcLocalPixPosY);

	return {srcDestPixPosX, srcDestPixPosY};
};

/**
 * @brief Blits an Image onto another at a specified position
 * 
 * @param src Source image.
 * @param dest Destination image.
 * @param posMap Position mapping between source and destination. Ignores widths and heights and uses those of the surfaces instead. 
 * @param scale_percentage Percentage points to scale the source by before blitting.
 * @return The return status code of the underlying SDL_BlitScaled function. 
 */
int blitImageConstAspectRatio(Image& src, Image& dest, PositionMapping posMap, uint scale_percentage = 100) {
	
	// The SDL gods demand a position sacrifice
	SDL_Rect pos;
	
	// Convenience variables
	uint srcSurfHeight = src.getSurface()->h;
	uint srcSurfWidth = src.getSurface()->w;


	// Create both ratios now, to enable easier comparison with the later src ratios
	double destWidthDivHeightRatio = static_cast<double>(dest.getSurface()->w) / static_cast<double>(dest.getSurface()->h);
	double destHeightDivWidthRatio = static_cast<double>(dest.getSurface()->h) / static_cast<double>(dest.getSurface()->w);

	// It's easier to define both the width/height and height/width ratios than it is to compute only one of the two, and then
	// take the reciprocal of it. Besides, in cases of surfaces with extreme ratios, even though we're using doubles, one might end up with
	// one of these being inaccurate. Note that this is purely theoretical, and that I reckon this will be fine for all practical purposes.
	// Still neater code-wise though, so I'm keeping this for the foreseeable future.
	double srcWidthDivHeightRatio = static_cast<double>(srcSurfWidth) / static_cast<double>(srcSurfHeight);
	double srcHeightDivWidthRatio = static_cast<double>(srcSurfHeight) / static_cast<double>(srcSurfWidth);
	
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
		pos.h = static_cast<uint>(std::round(scale * dest.getSurface()->h));
		pos.w = static_cast<uint>(std::round(scale * dest.getSurface()->h * srcWidthDivHeightRatio));
	}
	else {
		pos.h = static_cast<uint>(std::round(scale * dest.getSurface()->w * srcHeightDivWidthRatio));
		pos.w = static_cast<uint>(std::round(scale * dest.getSurface()->w));
	}

	// Filling in widths and heights
	posMap = {
		.srcPos = posMap.srcPos,
		.srcWidth = static_cast<uint>(pos.w),
		.srcHeight = static_cast<uint>(pos.h),

		.destPos = posMap.destPos,
		.destWidth = static_cast<uint>(dest.getSurface()->w),
		.destHeight = static_cast<uint>(dest.getSurface()->h)
	};
	// Get x and y coordinates of the screen to be blitted
	AbsolutePosition xy = getPixelPosfromPosition(posMap);

	

	pos.x = xy.x;
	pos.y = xy.y;


	return SDL_BlitScaled(src.getSurface(), nullptr, dest.getSurface(), &pos);
};




void renderText(SDL_Surface* screenSurface, TextBox& textBox, std::string text) {

	PositionMapping posMap = textBox.getPosMap();
	posMap = {
		.srcPos = posMap.srcPos,
		.srcWidth = static_cast<uint>(textBox.getBox()->w),
		.srcHeight = static_cast<uint>(textBox.getBox()->h),

		.destPos = posMap.destPos,
		.destWidth = static_cast<uint>(screenSurface->w),
		.destHeight = static_cast<uint>(screenSurface->h)
	};

	AbsolutePosition xy = getPixelPosfromPosition(posMap);

	SDL_Rect pos = {
		.x = xy.x,
		.y = xy.y,
		.w = textBox.getBox()->w,
		.h = textBox.getBox()->h,
	};

	SDL_BlitSurface(textBox.getBox(), nullptr, screenSurface, &pos);

	// TODO: make this relative to the screen
	pos.x += 30;
	pos.y += 30;

	SDL_BlitSurface(textBox.getText(), nullptr, screenSurface, &pos);
};

void renderFrame(SDLManager& SDLInfo, Frame& curFrame, TextBox textBox) {
	/* Render loop:
	   	
		Background
		Characters
			|- Position
			|- Expressions
			|		|- Expression builder?
			|- Animations?
		Dialogue
			|- Text box
			|- Text
			|- Autofitting
			|- Custom design?
		Graphical interface
			|- Keymaps
			|- Mouse clickies
			|- Menu
	*/

	// Initial setup
	SDL_Surface* screenSurface = SDLInfo.getScreenSurface();
	SDL_Window* window = SDLInfo.getWindow();
	Image& screen = SDLInfo.getScreenImage();

	

	// Background

	SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
	// Dimensions in posMaps are ignored in blitImageConstAspectRatio
	PositionMapping posMap = {
		.srcPos = {0.5, 0.5},
		.destPos = {0.5, 0.5},
	};
	
	blitImageConstAspectRatio(curFrame.bg, screen, posMap, 100);

	// Characters
	
	posMap = {
		.srcPos = {0.5, 1},
		.destPos = {0.5, 1},
	};
	#pragma GCC diagnostic pop
	blitImageConstAspectRatio(curFrame.storyCharacter.expressions.at(curFrame.expression), screen, posMap, 80);

	renderText(screen.getSurface(), textBox, curFrame.textDialogue);
	
	SDL_UpdateWindowSurface(window);

}

int handleEvents(Chapter& chapter) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN: {
				switch(event.key.keysym.sym) {
					case SDLK_ESCAPE: {
						return 137;
					}
					break;
					case SDLK_RIGHT:
					case SDLK_c:
					case SDLK_SPACE: {
						if (!event.key.repeat) {
							chapter.nextFrame();
						}
					}
					break;
					case SDLK_LEFT:
					case SDLK_z: {
						if (!event.key.repeat) {
							chapter.prevFrame();
						}
					}
					break;
				}
			}
			break;
		}
	}
	return 0;
}

int main() {
	
	SDLManager SDLInfo;
	
	SDL_Surface* screenSurface = SDLInfo.getScreenSurface();

	Chapter test = initTest(static_cast<uint>(screenSurface->w), static_cast<uint>(screenSurface->h));

	auto& it = test.curFrame;

	while (true) { 
		if (handleEvents(test) == 137) {
			return 0;
		}

		if (it == test.storyFrames.end()) {
			return 0;
		}
		assert(it.base() != nullptr);
		renderFrame(SDLInfo, *it.base(), test.textBox);

		SDL_Delay(10);
	}

	// TODO:
	// Render loop
	// Make a loader at some point, though after the main displayer works




	return 0;
}
