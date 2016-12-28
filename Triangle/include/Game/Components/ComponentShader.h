#pragma once
#include "Modules\ECS\Component.h"
#include "Modules\Graphics\Graphics.h"


namespace Components
{
	template<class AttributeType, class UniformType>
	class ComponentShader : public Component, public Graphics::Data::Shader<AttributeType, UniformType>
	{
		using ShaderType = Graphics::Data::Shader<AttributeType, UniformType>;
	public:
		ComponentShader(AttributeType&& attributes, UniformType&& uniforms) : ShaderType(std::move(attributes), std::move(uniforms)) {}
		ComponentShader(ComponentShader&& other) noexcept : Component(std::move(other)), ShaderType(std::move(other)) {}
	};
}