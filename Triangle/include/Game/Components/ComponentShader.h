#pragma once
#include "Modules/ECS/Component.h"
#include "Modules/Graphics/Shaders/Shader.h"

namespace Components
{
	template<class As, class Us>
	class ComponentShader : public ecs::Component, public graphics::shader_instance<As, Us>
	{
		using shader_t = graphics::shader_instance<As, Us>;
		using attributes_t = typename shader_t::attributes_t;
		using uniforms_t = typename shader_t::uniforms_t;

	public:
		ComponentShader(attributes_t attributes, uniforms_t uniforms) : 
			graphics::shader_instance<As, Us>(std::forward<attributes_t>(attributes), 
			std::forward<uniforms_t>(uniforms)) {}

		ComponentShader(ComponentShader&& other) : Component(std::move(other)), 
			graphics::shader_instance<As, Us>(std::move(other)) {}
	};
}