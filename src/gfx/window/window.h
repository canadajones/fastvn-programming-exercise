#ifndef VNPGE_SDL_WINDOW_HEADER
#define VNPGE_SDL_WINDOW_HEADER

#include <SDL2/SDL_render.h>
#include <cassert>
#include <memory>
#include <stdexcept>


#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>


namespace vnpge {
namespace gfx {
namespace window {


class SDLScope {
	private:
	static inline bool wasInit = false;
	
	public:
	SDLScope() {
		assert(!wasInit);

		if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
			throw std::runtime_error(SDL_GetError());
		}

		wasInit = true;
	}
	~SDLScope() {
		SDL_Quit();
	}

	SDLScope(const SDLScope&) = delete;
	SDLScope(const SDLScope&&) = delete;
	SDLScope& operator=(const SDLScope&) = delete;
	SDLScope& operator=(const SDLScope&&) = delete;
};

class Window {
	private:
	std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
	
	public:
	Window() : window{SDL_CreateWindow("test", 10, 10, 640, 480, SDL_WINDOW_SHOWN), SDL_DestroyWindow}{
		if (!window) {
			throw std::runtime_error(SDL_GetError());
		}		
	}
	
	SDL_Window* getWindow() {
		return window.get();
	}

};


class Renderer {
	private:
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;

    public:
    Renderer(Window& window) : renderer{SDL_CreateRenderer(window.getWindow(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC), SDL_DestroyRenderer} {
        if (!renderer) {
            throw std::runtime_error(SDL_GetError());
        }
    }

    SDL_Renderer* getRenderer() {
        return renderer.get();
    }

};



}
}
}


#endif