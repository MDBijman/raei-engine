#pragma once
#include "Modules\ECS\Component.h"
#include "Modules\Graphics\Graphics.h"


namespace Components
{
	using AttributeType = Graphics::Data::Attributes<Graphics::Data::Vec2<0, 0>, Graphics::Data::Vec2<1, sizeof(float) * 2>>;
	using UniformType = Graphics::Data::Uniforms<Graphics::Data::Texture<1, vk::ShaderStageFlagBits::eFragment>>;
	//using UniformType = Graphics::Data::Uniforms<>;
	using ShaderType = Graphics::Data::Shader<AttributeType, UniformType>;

	class SpriteShader : public Component, public ShaderType
	{
	public:
		/**
		* \brief Creates a new SpriteShader.
		* \param device The VulkanDevice to make vulkan objects with.
		* \param physicalDevice The VulkanPhysicalDevice to make vulkan objects.
		*/
		SpriteShader(Graphics::VulkanContext& context, AttributeType&& attributes, UniformType&& uniforms) : ShaderType(std::move(attributes), std::move(uniforms)) {}

		SpriteShader(SpriteShader&& other) noexcept : Component(std::move(other)), ShaderType(std::move(other)) {}

		/**
		* \brief Updates the uniform buffers for this shader.
		* \param camera The camera to calculate the model view projection matrix from.
		* \param device The VulkanDevice to map and unmap memory from.
		*/
		void updateUniformBuffers(Camera& camera, vk::Device& device) override
		{
			return;
		}
	};
}