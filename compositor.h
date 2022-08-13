#ifndef VNPGE_COMPOSITOR_HEADER
#define VNPGE_COMPOSITOR_HEADER

#include <functional>

#include "structures.h"


namespace vnpge {

template<typename RenderFunc>
class CompositorArea {
	public:
	RelativeArea area;

	std::function<bool()> shouldRender;

	RenderFunc renderToTarget;

	CompositorArea(const RelativeArea& area, std::function<bool()> shouldRender, RenderFunc render) :
		 area{area}, shouldRender{shouldRender}, renderToTarget{render} {};
};


}
#endif