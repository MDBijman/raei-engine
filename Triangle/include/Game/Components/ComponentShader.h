#pragma once
#include "Modules/ECS/Component.h"
#include "Modules/Graphics/Shaders/Shader.h"

namespace Components
{
	template<class As, class Us>
	class ComponentShader : public ecs::Component, public graphics::shader<As, Us>
	{
		using shader_t = graphics::shader<As, Us>;
		using attributes_t = typename shader_t::attributes_t;
		using indices_t = typename shader_t::indices_t;
		using uniforms_t = typename shader_t::uniforms_t;

	public:
		ComponentShader(attributes_t attributes, indices_t indices, uniforms_t uniforms) : 
			graphics::shader<As, Us>(std::forward<attributes_t>(attributes), std::forward<indices_t>(indices),
			std::forward<uniforms_t>(uniforms)) {}

		ComponentShader(ComponentShader&& other) : Component(std::move(other)), 
			graphics::shader<As, Us>(std::move(other)) {}
	};
}