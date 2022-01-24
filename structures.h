#ifndef STRUCTURES_HEADER 
#define STRUCTURES_HEADER

#include <iostream>

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

struct PositionedArea {
	public:
	AbsoluteDimensions area;
	AbsolutePosition position;
};

struct Colour {
	public:
	int red;
	int green;
	int blue;
};



};
#endif