#pragma once

/*
	Type index
*/

template<typename T, typename... Ts>
struct type_index;

template <typename T, typename... Ts>
struct type_index<T, T, Ts...> : std::integral_constant<std::size_t, 0>
{
};

template <typename T, typename Tail, typename... Ts>
struct type_index<T, Tail, Ts...> : std::integral_constant<std::size_t, 1 + type_index<T, Ts...>::value>
{
};

template<typename T>
struct type_index<T>
{
	static_assert(sizeof(T) == 0, "Type not found. Check your Config.");
};