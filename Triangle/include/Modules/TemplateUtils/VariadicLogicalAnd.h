#pragma once

namespace type_utils
{
	namespace detail
	{
		template <bool...>
		struct bool_pack { };
	}

	template<bool... Bs>
	using var_and = std::is_same<detail::bool_pack<true, Bs...>, detail::bool_pack<Bs..., true>>;

	template<bool... Bs>
	constexpr bool var_and_v = var_and<Bs...>::value;
}
