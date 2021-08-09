#include <functional>

#include "structures.h"
#include "character.h"
#include "image.h"

#ifndef TRANSFORMS_HEADER
#define TRANSFORMS_HEADER


namespace vnpge {

using Animator = std::function<void (Character&, AbsolutePosition&, AbsolutePosition&, double)>;
class Transform {
	private:
	AbsolutePosition start;
	AbsolutePosition end;
	double time;
	Animator animator;

	public:

	Transform(AbsolutePosition& start, AbsolutePosition& end, double time, Animator animator) : start{start}, end{end}, time{time},
	animator{animator} {
		
	};

};

};
#endif