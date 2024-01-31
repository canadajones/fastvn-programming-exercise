
#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_surface.h"
#include <SDL2/SDL_render.h>


#include "gfx/window/window.h"


namespace window = vnpge::gfx::window;
int main(int argc, char** argv) {
	window::SDLScope scope{};
	window::Window window{};
	window::Renderer renderer{window};

	SDL_Renderer* r = renderer.getRenderer();

	SDL_Surface* surf = SDL_CreateRGBSurface(0, 200, 200, 32, 0, 0, 0, 0);

	SDL_Rect rect =  {.x = 20, .y = 40, .w = 200, .h = 30};

	SDL_FillRect(surf, nullptr, 0xFFFFFFFF);
	SDL_FillRect(surf, &rect, 0x0C0C0CFF);

	SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
	
	SDL_SetRenderDrawColor(r, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(r);

	SDL_RenderCopy(r, tex, nullptr, &rect);

	SDL_RenderPresent(r);	

	SDL_Delay(7000);

	return 0;
	
	
}