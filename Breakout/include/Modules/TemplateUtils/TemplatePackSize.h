#pragma once

// TODO fix namespace and naming convention

template<class... T>
struct packSize;

template<>
struct packSize<>
{
	static const size_t value = 0;
};

template<class Head, class... T>
struct packSize<Head, T...>
{
	static const size_t value = sizeof(Head) + packSize<T...>::value;
};