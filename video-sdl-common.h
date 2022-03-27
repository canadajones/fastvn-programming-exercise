#ifndef VN_VIDEO_SDL_COMMON
#define VN_VIDEO_SDL_COMMON
#include <SDL2/SDL_video.h>

#include "structures.h"
#include "schedule.h"

namespace vnpge {
	SDL_Surface* makeNewSurface(uint w, uint h);
	
	Schedule<Event> handleEvents();
	AbsolutePosition getPixelPosfromPosition(AbsoluteDimensions& srcDim, AbsoluteDimensions& destDim, PositionMapping& posMap);
	
	std::string printRect(SDL_Rect& rect);
}
#endif