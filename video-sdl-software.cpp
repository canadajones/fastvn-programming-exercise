module;

#include <memory>
#include <vector>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "video-sdl-common.h"


#include "structures.h"
#include "schedule.h"

export module SoftwareRendering;
import Chapter;
import Image;
import SoftwareText;

namespace vnpge {
	namespace sw {
		class SDLManager {
			private:
			SDL_Window* window;
			SDL_Surface* screenSurface;
			Image screen;
			
			public:
			SDLManager();

			SDLManager(const SDLManager&) = delete;
			SDLManager operator=(const SDLManager&) = delete;

			~SDLManager();

			sw::SDLManager::SDLManager() {
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

		// Create Image container for the screen surface
		screen = Image(screenSurface, true);

	}

	sw::SDLManager::~SDLManager() {
		// Unload font support
		TTF_Quit();

		// Unload image format loading support
		IMG_Quit();
		
		// Destroy window
		SDL_DestroyWindow(window);
		
		// Quit SDL subsystems
		SDL_Quit();
	}

	SDL_Surface* sw::SDLManager::getScreenSurface() {
		return SDL_GetWindowSurface(window);
	}

	SDL_Window* sw::SDLManager::getWindow() {
		return window;
	}

	Image sw::SDLManager::getScreenImage() {
		return Image(SDL_GetWindowSurface(window), true);
	}

	};
};

/**
 * @brief Blits an Image onto another at a specified position.
 * 
 * @param src Source image.
 * @param dest Destination image.
 * @param posMap Relative position mapping between source and destination. 
 * @param scale_percentage Percentage points to scale the source by before blitting.
 * @return The return status code of the underlying SDL_BlitScaled function. 
 */
int blitImageConstAspectRatio(Image& src, Image& dest, PositionMapping posMap, uint scale_percentage = 100);


void renderText(SDL_Surface* screenSurface, TextBox& textBox, std::string text);

void renderFrame(sw::SDLManager& SDLInfo, Frame& curFrame, TextBox textBox);
}





/**
 * @brief Blits an Image onto another at a specified position
 * 
 * @param src Source image.
 * @param dest Destination image.
 * @param posMap Position mapping between source and destination. Ignores widths and heights and uses those of the surfaces instead. 
 * @param scale_percentage Percentage points to scale the source by before blitting.
 * @return The return status code of the underlying SDL_BlitScaled function. 
 */
int blitImageConstAspectRatio(Image& src, Image& dest, PositionMapping posMap, uint scale_percentage) {
	
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


	AbsoluteDimensions srcDim = {
		.w = static_cast<uint>(pos.w),
		.h = static_cast<uint>(pos.h)
	};

	AbsoluteDimensions destDim = {
		.w = static_cast<uint>(dest.getSurface()->w),
		.h = static_cast<uint>(dest.getSurface()->h)
	};
	// Get x and y coordinates of the screen to be blitted
	AbsolutePosition xy = getPixelPosfromPosition(srcDim, destDim, posMap);

	pos.x = xy.x;
	pos.y = xy.y;


	return SDL_BlitScaled(src.getSurface(), nullptr, dest.getSurface(), &pos);
};

void renderText(SDL_Surface* screenSurface, TextBox& textBox, std::string text) {

	PositionMapping posMap = textBox.getPosMap();
	posMap = {
		.srcPos = posMap.srcPos,
		.destPos = posMap.destPos
	};

	AbsoluteDimensions srcDim = {
		.w = static_cast<uint>(textBox.getBox()->w),
		.h = static_cast<uint>(textBox.getBox()->h),
	};

	AbsoluteDimensions destDim = {
		.w = static_cast<uint>(screenSurface->w),
		.h = static_cast<uint>(screenSurface->h)
	};
	AbsolutePosition xy = getPixelPosfromPosition(srcDim, destDim, posMap);

	SDL_Rect pos = {
		.x = xy.x,
		.y = xy.y,
		.w = textBox.getBox()->w,
		.h = textBox.getBox()->h,
	};

	SDL_BlitSurface(textBox.getBox(), nullptr, screenSurface, &pos);

	// TODO: make this relative to the screen
	pos.x += 20;
	pos.y += 20;

	SDL_BlitSurface(textBox.getText(), nullptr, screenSurface, &pos);
};

void renderFrame(sw::SDLManager& SDLInfo, Frame& curFrame, TextBox textBox) {
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
	SDL_Surface* screenSurface = SDLInfo.getScreenSurface();
	SDL_Window* window = SDLInfo.getWindow();
	Image screen = SDLInfo.getScreenImage();

	

	// Background

	SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
	
	PositionMapping posMap = {
		.srcPos = {0.5, 0.5},
		.destPos = {0.5, 0.5},
	};
	
	blitImageConstAspectRatio(curFrame.bg, screen, posMap, 100);

	// Characters
	
	// TODO: custom expression handlers
	blitImageConstAspectRatio(curFrame.storyCharacter.expressions.at(curFrame.expression), screen, curFrame.position, 80);

	// Text
	renderText(screen.getSurface(), textBox, curFrame.textDialogue);
	
	SDL_UpdateWindowSurface(window);

}
AbsoluteDimensions sw::SDLManager::getScreenDimensions() {
	return {static_cast<uint>(screenSurface->w), static_cast<uint>(screenSurface->h)};
};

};