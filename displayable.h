#ifndef DISPLAYABLE_HEADER
#define DISPLAYABLE_HEADER
#include <type_traits>
#include <concepts>

#include "imagebase.h"

namespace vnpge {




template<typename Renderable, typename RendererState> 
class Displayable {
	virtual Renderable renderDisplayable(RendererState) = 0;
};

template <typename T>
concept is_displayable = std::is_base_of_v<Displayable, T>;


};
#endif