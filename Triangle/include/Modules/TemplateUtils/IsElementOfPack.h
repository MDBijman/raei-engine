#pragma once
#include <type_traits>

template<class T, class... Ts>
struct is_element;

template<class T, class... Ts>
struct is_element<T, T, Ts...> : std::bool_constant<true> {};

template<class T, class Head, class... Ts>
struct is_element<T, Head, Ts...> : std::bool_constant<is_element<T, Ts...>::value> {};

template<class T>
struct is_element<T> : std::bool_constant<false> {};
