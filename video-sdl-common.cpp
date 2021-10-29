#include <vector>

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

}