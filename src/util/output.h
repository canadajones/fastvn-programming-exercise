#ifndef OUTPUT_DEBUG_HEADER
#define OUTPUT_DEBUG_HEADER

#include <iostream>

template <typename... Args>
inline void print(Args... args) {
    std::cout << (args << ...);
}



#endif