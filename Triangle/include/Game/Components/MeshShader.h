#pragma once
#include "Modules/ECS/ECS.h"
#include "Modules/Graphics/Graphics.h"
#include "Modules/Math/Matrices.h"
#include "Modules/Graphics/VulkanWrappers/VulkanContext.h"

namespace Components
{
	using MeshAttributeType = Graphics::Data::Attributes<
		Graphics::Data::Vec3<0, 0>, Graphics::Data::Vec2<1, 3>, Graphics::Data::Vec3<2, 5>
	>;

	using MeshUniformType = Graphics::Data::Uniforms<
		Graphics::Data::UniformBuffer<0, vk::ShaderStageFlagBits::eVertex>,
		Graphics::Data::Texture<1, vk::ShaderStageFlagBits::eFragment>
	>;

	using MeshShaderType = Graphics::Data::Shader<MeshAttributeType, MeshUniformType>;


	/**
	 * \brief MeshShader provides an implementation for the mesh fragment and vertex shaders.
	 */
	class MeshShader : public Component, public MeshShaderType
	{
	public:
		/**
		 * \brief Creates a new MeshShader with the required uniform data objects, descriptor set, descriptor set layout, and descriptor pool.
		 * \param device The VulkanDevice to make vulkan objects with.
		 * \param physicalDevice The VulkanPhysicalDevice to make vulkan objects.
		 */
		MeshShader(Graphics::VulkanContext& context, MeshAttributeType attributes, MeshUniformType uniforms) : MeshShaderType(std::move(attributes), std::move(uniforms))
		{
			allocateUniform(context);
		}

		MeshShader(MeshShader&& other) noexcept : Component(std::move(other)), MeshShaderType(std::move(other))
		{

		}


	private:
		void allocateUniform(Graphics::VulkanContext& context)
		{

		}

		Graphics::Data::Uniform		uniformDataVS;
		Graphics::Data::UniformMVP	uboVS;
	};
}
