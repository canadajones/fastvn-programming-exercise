#ifndef VN_VIDEO_SDL_COMMON
#define VN_VIDEO_SDL_COMMON
#include "structures.h"
#include "schedule.h"

namespace vnpge {
	Schedule<Event> handleEvents();
	AbsolutePosition getPixelPosfromPosition(AbsoluteDimensions& srcDim, AbsoluteDimensions& destDim, PositionMapping& posMap);
}
#endif