#pragma once
#include <variant>

namespace ECS
{
	using TypeVariant = std::variant<int, const char*, void*, double>;

	// Trait to implement to allow entity/component construction from Lua
	template<class T>
	class FromLua;
}