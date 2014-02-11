#ifndef type_traits_h
#define type_traits_h

namespace utility {

template<typename T> struct is_lvalue_reference { static constexpr auto value = false; };
template<typename T> struct is_lvalue_reference<T&> { static constexpr auto value = true; };

template<typename T> struct is_rvalue_reference { static constexpr auto value = false; };
template<typename T> struct is_rvalue_reference<T&&> { static constexpr auto value = true; };

template<typename T> struct remove_reference { using type = T; };
template<typename T> struct remove_reference<T&> { using type = T; };

template<typename T> struct remove_const { using type = T; };
template<typename T> struct remove_const<const T> { using type = T; };

template<typename T> struct remove_volatile { using type = T; };
template<typename T> struct remove_volatile<volatile T> { using type = T; };

template<typename T> struct remove_cv {
    using type = typename remove_volatile<typename remove_const<T>::type>::type;
};

} /* namespace utility */

#endif /* type_traits_h */
