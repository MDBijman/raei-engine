#pragma once
#include <type_traits>

template<class T, class H, class... Ts>
constexpr bool is_element()
{
	if constexpr(std::is_same_v<T, H>) return true;
	else if constexpr(sizeof...(Ts) == 0) return false;
	else return is_element<T, Ts...>();
}
