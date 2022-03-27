#include <SDL2/SDL_error.h>
#include <stdexcept>
#include <vector>
#include <string>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

#include "schedule.h"

namespace vnpge {

// Nullary function that's handy for use as a breakpoint hook. In the debugger, set a breakpoint on it and step until you've returned
// If using gdb, typing 'break brkpoint()' will accomplish this
void brkpoint() {
	// Keeping a copy of this here in case I need some good ol' print-debugging on which execution path is taken
	std::cout << __FILE__ << " " << __LINE__ << std::endl;
	return;
}

AbsolutePosition getPixelPosfromPosition(AbsoluteDimensions& srcDim, AbsoluteDimensions& destDim, PositionMapping& posMap) {

	// It contains a pair of points, plus the widths and heights of the two surfaces
	// The points are in the form of normalised doubles, where 0,0 is the top left corner,
	// and 1,1 is the bottom right corner.
	// Each has been normalised according to its own surface's width and height, so we first have to unpack this into pixels
	// We need the pixels because we need to have a unit of distance which is the same distance in both coordinate systems
	// Simply using the doubles would give an incorrect result, as it would be equivalent to dest_x/dest/width - src_x/src_width,
	// which I hope is obvious wouldn't work. You can't just add and subtract percentages!

	int srcLocalPixPosX = srcDim.w * posMap.srcPos.x;

	int srcLocalPixPosY = srcDim.h * posMap.srcPos.y;

	int destLocalPixPosX = destDim.w * posMap.destPos.x;

	int destLocalPixPosY = destDim.h * posMap.destPos.y;

	// Now that we have the variables unpacked, use them
	// Technically, the src coordinates here are being used for their distance from the origin
	// But since the taxicab distance and numerical value of the point is the same, it works just fine

	int srcDestPixPosX = static_cast<uint>(destLocalPixPosX - srcLocalPixPosX);
	int srcDestPixPosY = static_cast<uint>(destLocalPixPosY - srcLocalPixPosY);

	return { srcDestPixPosX, srcDestPixPosY };
};
SDL_Surface* makeTextBox(AbsoluteDimensions pixelDimensions, RelativeDimensions relDimensions) {
	// Default simple text box
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	SDL_Surface* textBoxSurface = SDL_CreateRGBSurface(0, pixelDimensions.w*relDimensions.w, pixelDimensions.h*relDimensions.h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	#else
	SDL_Surface* textBoxSurface = SDL_CreateRGBSurface(0, pixelDimensions.w*relDimensions.w, pixelDimensions.h*relDimensions.h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	#endif
	
	if (textBoxSurface == nullptr) {
		std::string err = "Surface could not be created! SDL_Error: "; 
		throw std::runtime_error(err.append(SDL_GetError()));
	}

	SDL_Rect textBox = {
		.x = 0,
		.y = 0,
		.w = textBoxSurface->w,
		.h = textBoxSurface->h
	};

	// Fill with a transparent white
	SDL_FillRect(textBoxSurface, &textBox, SDL_MapRGBA(textBoxSurface->format, 0xFF, 0xFF, 0xFF, 0x7F));

	textBox.x += 4;
	textBox.y += 4;
	textBox.w -= 8;
	textBox.h -= 8;

	// Fill with a transparent black inside the other rectangle, thereby creating a white border
	// Note that FillRect does not blend alphas, so this alpha replaces the white's
	// This is actually desirable, since the colours should have different alphas in order to feel equally transparent
	SDL_FillRect(textBoxSurface, &textBox, SDL_MapRGBA(textBoxSurface->format, 0x30, 0x30, 0x30, 0xAF));

	return textBoxSurface;
};

SDL_Surface* makeNewSurface(uint w, uint h) {
	SDL_Surface* surf;
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	surf = SDL_CreateRGBSurface(0, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	#else
	surf = SDL_CreateRGBSurface(0, w, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	#endif
	if (surf == nullptr) {
		std::string err = "Could not create a SDL_Surface due to: ";
		throw std::runtime_error(err.append(SDL_GetError()));
	}
	return surf;
};

Schedule<Event> handleEvents() {
	std::vector<Event> events;

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN: {
				switch(event.key.keysym.sym) {
					case SDLK_ESCAPE: {
						// 137; magic number for program exit
						events.emplace_back(Action::clean_exit);
					}
					break;
					case SDLK_RIGHT:
					case SDLK_c:
					case SDLK_SPACE: {
						if (!event.key.repeat) {
							events.emplace_back(Action::next_page);
						}
					}
					break;
					case SDLK_LEFT:
					case SDLK_z: {
						if (!event.key.repeat) {
							events.emplace_back(Action::prev_page);
						}
					}
					break;

					case SDLK_UP:{
						if (!event.key.repeat) {
							events.emplace_back(Action::scroll_up);
						}
					}
					break;
					case SDLK_DOWN:{
						if (!event.key.repeat) {
							events.emplace_back(Action::scroll_down);
						}
					}
					break;
				}
			}
			break;
			case SDL_WINDOWEVENT: {
				switch(event.window.event) {
					case SDL_WINDOWEVENT_RESIZED: {
						events.emplace_back(Action::window_resized, event.window.data1, event.window.data2);
					}
					break;
					case SDL_WINDOWEVENT_CLOSE: {
						events.emplace_back(Action::clean_exit);
					}
				}
			}
			break;
		}
	}
	Schedule<Event> evSched = {events};
	return evSched;
};


std::string printRect(SDL_Rect& rect) {
	return { "x: " + std::to_string(rect.x) + ", y: " + std::to_string(rect.y) +
		", w: " + std::to_string(rect.w) + ", h: " + std::to_string(rect.h) };
}

}