#ifndef VN_VIDEO_SDL_SW_HEADER
#define VN_VIDEO_SDL_SW_HEADER

#include <memory>
#include <vector>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>

#include <SDL2/SDL_image.h>

#include <SDL2/SDL_ttf.h>

#include "structures.h"
#include "chapter.h"
#include "image.h"
#include "text.h"
#include "schedule.h"

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

			SDL_Surface* getScreenSurface();
			SDL_Window* getWindow();
			Image getScreenImage();
			AbsoluteDimensions getScreenDimensions();
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

#endif