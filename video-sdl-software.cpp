module;

#include <memory>
#include <vector>
#include <string>
#include <exception>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "video-sdl-common.h"


#include "structures.h"
#include "schedule.h"


export module SoftwareRender;

import Chapter;
import Image;
import StoryDialogue;
import SoftwareText;


export namespace vnpge {

template<typename T>
void nullDeleter(T* surf) {
};

class SoftwareImage : Image {
private:
	std::shared_ptr<SDL_Surface> surf;
public:
	SoftwareImage(Image& baseImage) : Image{ baseImage }, surf{ IMG_Load(baseImage.path.c_str()), SDL_FreeSurface } {
		std::cout << "new image loaded" << std::endl; 
	};

	SoftwareImage(SDL_Surface* surf) : Image{ "undefined" }, surf{ surf, SDL_FreeSurface} {};

	SoftwareImage(SDL_Surface* surf, bool useNullDeleter) : Image{ "undefined" }, surf{ surf, nullDeleter<SDL_Surface> } {};

	SDL_Surface* getSurface() {
		return surf.get();
	}
};

class SWRenderManager {
private:
	SDL_Window* window;
	SDL_Surface* screenSurface;
	
	std::unordered_map<std::string, SoftwareImage> imageMap;
public:

	SWRenderManager() {
		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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
			std::string err = "TTF_Init: ";
			throw std::runtime_error(err.append(TTF_GetError()));
		}

		//Create window
		//window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 800, SDL_WINDOW_FULLSCREEN_DESKTOP);

		window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 600, SDL_WINDOW_RESIZABLE);

		// If window creation failed, crash and burn
		if (window == nullptr) {
			std::string err = "Window could not be created! SDL_Error: ";
			throw std::runtime_error(err.append(SDL_GetError()));
		}

		screenSurface = SDL_GetWindowSurface(window);
	};

	~SWRenderManager() {
		// Unload font support
		TTF_Quit();

		// Unload image format loading support
		IMG_Quit();

		// Destroy window
		SDL_DestroyWindow(window);

		// Quit SDL subsystems
		SDL_Quit();
	};

	SDL_Surface* getScreenSurface() {
		screenSurface = SDL_GetWindowSurface(window);
		return screenSurface;
	};

	SDL_Window* getWindow() {
		return window;
	};

	SoftwareImage getScreenImage() {
		screenSurface = SDL_GetWindowSurface(window);
		return { screenSurface, true };
	};

	AbsoluteDimensions getScreenDimensions() {
		screenSurface = SDL_GetWindowSurface(window);
		return { static_cast<uint>(screenSurface->w), static_cast<uint>(screenSurface->h) };
	};

	SoftwareImage& getImage(Image& image) {
		if (imageMap.contains(image.path)) {
			return imageMap.at(image.path);
		}
		imageMap.insert({image.path, {image}});
		return imageMap.at(image.path);
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
int blitImageConstAspectRatio(SoftwareImage src, SoftwareImage dest, PositionMapping posMap, uint scale_percentage) {

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





void renderText(TextRenderer& textRenderer, SWRenderManager& renderManager, Dialogue dialogue, DialogueFont font) {
	textRenderer.renderStoryFrame(dialogue, font);
	// hack
	AbsoluteDimensions d = renderManager.getScreenDimensions();
	textRenderer.displayText({.x = 0, .y = static_cast<int>(0.75 * d.h)});
	
};


void renderFrame(SWRenderManager& SDLInfo, Frame& curFrame, TextRenderer& textRenderer) {
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
	SoftwareImage screen = SDLInfo.getScreenImage();

	

	// Background
	std::cout << "background" << std::endl;

	SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
	
	PositionMapping posMap = {
		.srcPos = {0.5, 0.5},
		.destPos = {0.5, 0.5},
	};
	
	if (blitImageConstAspectRatio(SDLInfo.getImage(curFrame.bg), screen, posMap, 100)) {
		std::string err = "SDL error! Error string is ";
		throw std::runtime_error(err.append(SDL_GetError()));
	}

	// Characters
	std::cout << "characters" << std::endl;
	// TODO: custom expression handlers 
	if (blitImageConstAspectRatio(SDLInfo.getImage(curFrame.storyCharacter.expressions.at(curFrame.expression)), screen, curFrame.position, 80)) {
		std::string err = "SDL error! Error string is ";
		throw std::runtime_error(err.append(SDL_GetError()));
	}
	
	// Text
	std::cout << "text" << std::endl;
	renderText(textRenderer, SDLInfo, {curFrame.storyCharacter.name, curFrame.textDialogue, {255, 255, 255}}, {"BonaNova-Italic.ttf"});
	
	std::cout << "flip buffers" << std::endl;
	SDL_UpdateWindowSurface(window);

}


};