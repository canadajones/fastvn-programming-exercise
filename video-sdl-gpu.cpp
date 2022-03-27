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


export module AcceleratedRender;

import Chapter;
import Image;
import StoryDialogue;
import AcceleratedText;


export namespace vnpge {

template<typename T>
void nullDeleter(T* surf) {
};

class GPUImage : Image {
private:
	std::shared_ptr<SDL_Texture> texture;
public:
	GPUImage(SDL_Renderer* renderer, const Image& baseImage) : Image{ baseImage } {
		
		std::cout << "load file" << std::endl;
		SDL_Surface* surf = IMG_Load(baseImage.path.c_str());
		
		std::cout << "generate texture" << std::endl;

		texture = {SDL_CreateTextureFromSurface(renderer, surf), SDL_DestroyTexture};

		SDL_FreeSurface(surf);

		std::cout << "new texture loaded" << std::endl; 
	};

	SDL_Texture* getTexture() {
		return texture.get();
	}
};

class GPURenderManager {
private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	std::unordered_map<std::string, GPUImage> imageMap;
public:
	GPURenderManager() {
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

		
		std::cout << ((SDL_SetHintWithPriority(SDL_HINT_RENDER_SCALE_QUALITY, "2", SDL_HINT_OVERRIDE) == SDL_TRUE) ? "antialias accepted" : "no aa") << std::endl;
		SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengl", SDL_HINT_OVERRIDE);

		//Create window
		//window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 800, SDL_WINDOW_FULLSCREEN_DESKTOP);

		window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 600, SDL_WINDOW_RESIZABLE);

		// If window creation failed, crash and burn
		if (window == nullptr) {
			std::string err = "Window could not be created! SDL_Error: ";
			throw std::runtime_error(err.append(SDL_GetError()));
		}

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		if (renderer == nullptr) {
			std::string err = "Renderer could not be created! SDL_Error: ";
			throw std::runtime_error(err.append(SDL_GetError()));
		}
		std::cout << SDL_GetHint(SDL_HINT_RENDER_SCALE_QUALITY) << std::endl;
	};

	~GPURenderManager() {
		// Unload font support
		TTF_Quit();

		// Unload image format loading support
		IMG_Quit();

		// Destroy window
		SDL_DestroyWindow(window);

		// Quit SDL subsystems
		SDL_Quit();
	};


	SDL_Window* getWindow() {
		return window;
	};

	SDL_Renderer* getRenderer() {
		return renderer;
	}

	AbsoluteDimensions getScreenDimensions() {
		int w, h;
		if (SDL_GetRendererOutputSize(renderer, &w, &h)) {
			std::string err = "Renderer size not available! SDL_Error: ";
			throw std::runtime_error(err.append(SDL_GetError()));
		}

		return { static_cast<uint>(w), static_cast<uint>(h) };
	};

	GPUImage& getImage(Image& image) {
		if (imageMap.contains(image.path)) {
			return imageMap.at(image.path);
		}
		imageMap.insert({image.path, {renderer, image}});
		return imageMap.at(image.path);
	};

	/**
	* @brief Renders an Image at a specified position.
	*
	* @param src Source image.
	* @param posMap Relative position mapping between source and destination.
	* @param scale_percentage Percentage points to scale the source by before rendering.
	* @return The return status code of the underlying SDL_RenderCopy function.
	*/
	int renderImage(Image src, PositionMapping posMap, uint scale_percentage) {

		// The SDL gods demand a position sacrifice
		SDL_Rect pos;

		// Query image in cache
		GPUImage image = getImage(src);

		// Get texture width and height
		int w, h;
		SDL_QueryTexture(image.getTexture(), nullptr, nullptr, &w, &h);

		SDL_SetTextureBlendMode(image.getTexture(), SDL_BLENDMODE_BLEND);

		// Convenience variables
		uint srcTextureWidth	= w;
		uint srcTextureHeight	= h;

		uint rendererWidth		= getScreenDimensions().w;
		uint rendererHeight		= getScreenDimensions().h;

		// Create both ratios now, to enable easier comparison with the later src ratios
		double destWidthDivHeightRatio = static_cast<double>(rendererWidth) / static_cast<double>(rendererHeight);
		double destHeightDivWidthRatio = static_cast<double>(rendererHeight) / static_cast<double>(rendererWidth);

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
		if (srcHeightDivWidthRatio > destHeightDivWidthRatio || srcWidthDivHeightRatio < destWidthDivHeightRatio ) {
			pos.w = static_cast<uint>(std::round(scale * rendererHeight * srcWidthDivHeightRatio));
			pos.h = static_cast<uint>(std::round(scale * rendererHeight));
		}
		else {
			pos.w = static_cast<uint>(std::round(scale * rendererWidth));
			pos.h = static_cast<uint>(std::round(scale * rendererWidth * srcHeightDivWidthRatio));
		}


		AbsoluteDimensions srcDim = {
			.w = static_cast<uint>(pos.w),
			.h = static_cast<uint>(pos.h)
		};

		AbsoluteDimensions destDim = {
			.w = static_cast<uint>(rendererWidth),
			.h = static_cast<uint>(rendererHeight)
		};
		// Get x and y coordinates of the screen to be blitted
		AbsolutePosition xy = getPixelPosfromPosition(srcDim, destDim, posMap);

		pos.x = xy.x;
		pos.y = xy.y;


		return SDL_RenderCopyEx(renderer, image.getTexture(), nullptr, &pos, 0, nullptr, SDL_FLIP_NONE);
	};
};




void renderText(TextRenderer& textRenderer, GPURenderManager& renderManager, Dialogue dialogue, DialogueFont font) {
	textRenderer.renderStoryFrame(dialogue, font);
	// hack
	AbsoluteDimensions d = renderManager.getScreenDimensions();
	textRenderer.displayText({.x = 0, .y = static_cast<int>(0.75 * d.h)});
	
};


void renderFrame(GPURenderManager& SDLInfo, Frame& curFrame, TextRenderer& textRenderer) {
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

	

	// Background
	std::cout << "background" << std::endl;

	SDL_SetRenderDrawColor(SDLInfo.getRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
	

	SDL_RenderClear(SDLInfo.getRenderer());

	PositionMapping posMap = {
		.srcPos = {0.5, 0.5},
		.destPos = {0.5, 0.5},
	};
	
	if (SDLInfo.renderImage(curFrame.bg, posMap, 100)) {
		std::string err = "SDL error! Error string is ";
		throw std::runtime_error(err.append(SDL_GetError()));
	}

	// Characters
	std::cout << "characters" << std::endl;
	// TODO: custom expression handlers 
	if (SDLInfo.renderImage(curFrame.storyCharacter.expressions.at(curFrame.expression), curFrame.position, 80)) {
		std::string err = "SDL error! Error string is ";
		throw std::runtime_error(err.append(SDL_GetError()));
	}
	
	// Text
	std::cout << "text" << std::endl;
	renderText(textRenderer, SDLInfo, {curFrame.storyCharacter.name, curFrame.textDialogue, {255, 255, 255}}, {"assets/fonts/BonaNova-Italic.ttf"});
	
	std::cout << "flip buffers" << std::endl;
	SDL_RenderPresent(SDLInfo.getRenderer());

}


};