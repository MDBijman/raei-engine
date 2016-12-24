#pragma once
#include <vector>

template<class... T>
constexpr size_t getTemplatePackSize()
{
	std::array<size_t, sizeof...(T)> sizes{
		sizeof(T)...
	};

	return sumArray(sizes);
}

template<int N>
constexpr size_t sumArray(std::array<size_t, N> sizes)
{
	size_t size = 0;
	for(size_t s : sizes)
	{
		size += s;
	}
	return size;
}