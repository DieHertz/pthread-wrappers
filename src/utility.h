#ifndef utility_h
#define utility_h

#include "type_traits.h"

namespace utility {

/**
    \fn move
    \brief std::move implementation for compilers without <utility> header
*/
template<typename T> constexpr typename remove_reference<T>::type&& move(T&& t) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(t);
}

/**
    \fn forward
    \brief std::forward implementation
*/
template<typename T> constexpr T&& forward(typename remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}

template<typename T> constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept {
    static_assert(!is_lvalue_reference<T>::value, "template argument substituting T is an lvalue reference type");
    return static_cast<T&&>(t);
}

} /* namespace utility */

#endif /* utility_h */
