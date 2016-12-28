#pragma once
#include "Modules/Graphics/Graphics.h"
#include "ComponentShader.h"

namespace Components
{
	using SpriteAttributes = Graphics::Data::Attributes<Graphics::Data::Vec2<0, 0>, Graphics::Data::Vec2<1, sizeof(float) * 2>>;
	using SpriteUniforms = Graphics::Data::Uniforms<Graphics::Data::Texture<1, vk::ShaderStageFlagBits::eFragment>>;
	using SpriteShader = ComponentShader<SpriteAttributes, SpriteUniforms>;
}