#pragma once

namespace type_utils
{
	template<typename T, class = void>
	struct is_defined 
	{
		constexpr static bool value = false;
	};

	template<typename T>
	struct is_defined<T,
				std::enable_if_t<std::is_object<T>::value &&
				!std::is_pointer<T>::value &&
				(sizeof(T) > 0)
			>
		>
	{
		constexpr static bool value = true;
	};
}