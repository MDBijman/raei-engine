#pragma once

namespace utils
{
	template<class Head, class... T>
	constexpr std::size_t pack_size()
	{
		return sizeof(Head) + (sizeof...(T) == 0 ? 0 : pack_size<T...>());
	}
}
