#ifndef VN_VIDEO_SDL_GPU_HEADER
#define VN_VIDEO_SDL_GPU_HEADER
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
		SDL_Surface* screenSurface;
		Image screen;
		std::shared_ptr<SDL_Renderer> renderer;
		
		public:
		SDLManager();

		SDLManager(const SDLManager&) = delete;
		SDLManager operator=(const SDLManager&) = delete;

		~SDLManager();


		SDL_Surface* getScreenSurface();
		SDL_Window* getWindow();
		SDL_Renderer* getRenderer();
		std::shared_ptr<SDL_Renderer> getRendererShared();
		Image getScreenImage();
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
int renderTextureConstAspectRatio(SDL_Renderer* renderer, SDL_Texture* src, vnpge::PositionMapping posMap, uint scale_percentage = 100);

void renderText(SDL_Surface* screenSurface, vnpge::TextBox& textBox, std::string text);

void renderFrame(gpu::SDLManager& SDLInfo, vnpge::Frame& curFrame, vnpge::TextBox textBox);

class TexContainer {
	public:
	std::shared_ptr<SDL_Texture> bg;
	std::shared_ptr<SDL_Texture> character;
};

void renderTextAccel(SDL_Renderer* renderer, vnpge::TextBox& textBox, std::string text);
void renderFrameAccel(SDLManager& SDLInfo, vnpge::Frame& curFrame, TexContainer& texCon, vnpge::TextBox textBox);
Schedule<Event> handleEvents();
}
#endif