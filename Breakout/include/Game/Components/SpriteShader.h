#pragma once
#include "Modules/Graphics/Shaders/Attributes/Vec2.h"
#include "Modules/Graphics/Shaders/Uniforms/UniformBuffer.h"
#include "Modules/Graphics/Shaders/Attributes/Attributes.h"
#include "Modules/Graphics/Shaders/Uniforms/Uniforms.h"
#include "Modules/Graphics/Shaders/Uniforms/Texture.h"
#include "Modules/Graphics/Shaders/Shader.h"

namespace Components
{
	using sprite_attributes = graphics::data::attributes<
		graphics::data::Vec2<0, 0>, 
		graphics::data::Vec2<1, sizeof(float) * 2>
	>;

	using sprite_indices = typename graphics::data::indices;

	using sprite_uniforms = graphics::data::Uniforms<
		graphics::data::buffer_template<glm::mat4, 0, vk::ShaderStageFlagBits::eVertex>, 
		graphics::data::texture_template<1, vk::ShaderStageFlagBits::eFragment>
	>;

	using sprite_shader = graphics::shader<sprite_attributes, sprite_uniforms>;
}