#pragma once
#include "ComponentShader.h"
#include "Modules/Graphics/Shaders/Attributes/Vec2.h"
#include "Modules/Graphics/Shaders/Uniforms/UniformBuffer.h"
#include "Modules/Graphics/Shaders/Attributes/Attributes.h"
#include "Modules/Graphics/Shaders/Uniforms/Uniforms.h"

namespace Components
{
	using SpriteAttributes = Graphics::Data::Attributes<Graphics::Data::Vec2<0, 0>, Graphics::Data::Vec2<1, sizeof(float) * 2>>;
	using SpriteUniforms = Graphics::Data::Uniforms<Graphics::Data::UniformBuffer<glm::mat4, 0, vk::ShaderStageFlagBits::eVertex>, Graphics::Data::Texture<1, vk::ShaderStageFlagBits::eFragment>>;
	using SpriteShader = ComponentShader<SpriteAttributes, SpriteUniforms>;
}