#ifndef VNPGE_SDL_COMPOSITOR_HEADER
#define VNPGE_SDL_COMPOSITOR_HEADER
#include <memory>

#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"

#include "gfx/window/window.h"
#include "util/structures.h"

namespace vnpge {
namespace gfx {
namespace compositor {

namespace window = vnpge::gfx::window;

class CompositorArea {
	private:
	window::Renderer renderer;
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture;
	

	public:
	CompositorArea(window::Renderer&& renderer, AbsoluteDimensions dim) : renderer{std::move(renderer)}, texture{SDL_CreateTexture(this->renderer.getRenderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, dim.w, dim.h), SDL_DestroyTexture} {

	};

};


}
}
}
#endif