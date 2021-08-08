
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_ttf.h>


#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_image.h>

#ifndef STRUCTURES_HEADER 
#define STRUCTURES_HEADER

// Nullary function that's handy for use as a breakpoint hook. In the debugger, set a breakpoint on it and step until you've returned
// If using gdb, typing 'break brkpoint()' will accomplish this
// NOLINTNEXTLINE(misc-definitions-in-headers)
void brkpoint() {
	// Keeping a copy of this here in case I need some good ol' print-debugging on which execution path is taken
	std::cout << __FILE__ << " " << __LINE__ << std::endl;
	return;
}

namespace vnpge {


// In the interest of documentation:
// Declare things as struct if they could just as easily be constructed as a simple aggregate structure.
// More formally, if the class
// - does not provide any member or static functions, 
// - does not declare any destructors or move/copy constructors,
// - does not substantially check input for correctness in a way that a correctly coded external initialiser could miss 
// then it may be declared as struct.
// Otherwise, declare as class.
// This doesn't actually matter in any way, since all access specifiers must be explicit.
// But it allows for ascertaining whether a class is "simple" at a glance.

enum struct position : uint {
	left,
	right,
	middle_bottom,
	top,
	bottom
};
 

struct AbsolutePosition {
	public:
	int x;
	int y;
	
};

struct RelativePosition {
	public:
	double x;
	double y;
};

struct AbsoluteDimensions {
	public:
	uint w;
	uint h;
};

struct RelativeDimensions {
	public:
	double w;
	double h;
};

struct PositionMapping {
	public:
	RelativePosition srcPos;
	RelativePosition destPos;
};

// NOLINTNEXTLINE(misc-definitions-in-headers)
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

	return {srcDestPixPosX, srcDestPixPosY};
};


};
#endif