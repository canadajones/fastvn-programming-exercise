

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"

#include "gfx/window/window.h"


namespace window = vnpge::gfx::window;
int main(int argc, char** argv) {
	window::SDLScope scope{};
	window::Window window{};
	window::Renderer renderer{window};

	SDL_Renderer* r = renderer.getRenderer();

	SDL_Surface* surf = SDL_CreateSurface(200, 200, SDL_PIXELFORMAT_RGBA32);

	SDL_Rect rect =  {.x = 20, .y = 40, .w = 200, .h = 30};

	SDL_FillSurfaceRect(surf, nullptr, SDL_MapRGB(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32), nullptr, 0xFF, 0xFF, 0xFF));
	SDL_FillSurfaceRect(surf, &rect, SDL_MapRGB(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32), nullptr, 0x0C, 0x0C, 0x0C));

	SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
	
	SDL_SetRenderDrawColor(r, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(r);

	SDL_FRect frect;

	SDL_RectToFRect(&rect, &frect);

	SDL_RenderTexture(r, tex, nullptr, &frect);

	SDL_RenderPresent(r);	

	SDL_Delay(7000);

	return 0;
	
	
}