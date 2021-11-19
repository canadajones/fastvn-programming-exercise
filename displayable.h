#ifndef DISPLAYABLE_HEADER
#define DISPLAYABLE_HEADER
#include <type_traits>

#include "imagebase.h"

namespace vnpge {

template <typename T>
concept is_displayable = std::is_base_of_v<ImageBase, T>;

};
#endif