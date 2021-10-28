#ifndef VN_VIDEO_SDL_GPU_HEADER
#define VN_VIDEO_SDL_GPU_HEADER
#include <SDL2/SDL_render.h>
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

namespace vnpge {

namespace gpu {
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
			//window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 800, SDL_WINDOW_FULLSCREEN_DESKTOP);

			window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 600, SDL_WINDOW_RESIZABLE);

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
		};


		SDL_Window* getWindow() {
			return window;
		};

		SDL_Renderer* getRenderer() {
			return renderer.get();
		};
		std::shared_ptr<SDL_Renderer> getRendererShared() {
			return renderer;
		};
		AbsoluteDimensions getScreenDimensions() {
			int w, h;
			SDL_GetRendererOutputSize(renderer.get(), &w, &h);
			return {static_cast<uint>(w),static_cast<uint>(h)};
		};
	};
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
int renderTextureConstAspectRatio(SDL_Renderer* renderer, SDL_Texture* src, vnpge::PositionMapping posMap, uint scale_percentage);

void renderText(SDL_Surface* screenSurface, vnpge::TextBox& textBox, std::string text);

void renderFrame(gpu::SDLManager& SDLInfo, vnpge::Frame& curFrame, vnpge::TextBox textBox);

class TexContainer {
	public:
	std::shared_ptr<SDL_Texture> bg;
	std::shared_ptr<SDL_Texture> character;
};

void renderTextAccel(SDL_Renderer* renderer, vnpge::TextBox& textBox, std::string text);
void renderFrameAccel(gpu::SDLManager& SDLInfo, vnpge::Frame& curFrame, TexContainer& texCon, vnpge::TextBox textBox);
Schedule<Event> handleEvents();
AbsolutePosition getPixelPosfromPosition(AbsoluteDimensions& srcDim, AbsoluteDimensions& destDim, PositionMapping& posMap);

}

#endif