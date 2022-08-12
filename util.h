#ifndef VNPGE_UTILITY_HEADER
#define VNPGE_UTILITY_HEADER
#include <concepts>
#include <iterator>
#include <ranges>

template <std::ranges::bidirectional_range C>
struct reverse_wrapper {

    C& c_;
    reverse_wrapper(C& c) : c_(c) {}

    typename C::reverse_iterator begin() { return c_.rbegin(); }
    typename C::reverse_iterator end() { return c_.rend(); }
};


template <typename C>
reverse_wrapper<C> r_wrap(C& c) {
    return reverse_wrapper<C>(c);
}


#endif