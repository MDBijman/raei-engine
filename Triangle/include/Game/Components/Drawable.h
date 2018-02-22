#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include "Modules/ECS/Component.h"
#include "Modules/Graphics/Graphics.h"

namespace components
{
	template<class Shader>
	class drawable : public ecs::Component, public speck::graphics::drawable<Shader>
	{
	public:
		drawable(speck::graphics::drawable<Shader> d) : speck::graphics::drawable<Shader>(std::move(d)) {}
	};
}