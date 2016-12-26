#pragma once
#include "Modules/Graphics/Data/Shaders/Attributes.h"
#include "Modules/Graphics/Data/Shaders/Uniforms.h"
#include "Modules/Graphics/Data/Shaders/Texture.h"

#include <vulkan/vulkan.hpp>

namespace Graphics
{

	namespace Data
	{
		template<class A, class U>
		class Shader;

		template<class... A, class... U>
		class Shader<Attributes<A...>, Uniforms<U...>>
		{
		public:
			Shader(Attributes<A...>&& attributes, Uniforms<U...>&& uniforms) : attributes(std::move(attributes)), uniforms(std::move(uniforms))
			{}

			Shader(Shader&& other) : attributes(std::move(other.attributes)), uniforms(std::move(other.uniforms))
			{}

			const vk::DescriptorSetLayout& getLayout()
			{
				return uniforms.getDescriptorSetLayout();
			}

			const vk::DescriptorSet& getDescriptorSet()
			{
				return uniforms.getDescriptorSet();
			}

			const vk::PipelineVertexInputStateCreateInfo& getVI()
			{
				return attributes.getVI();
			}

			const Attributes<A...>& getAttributes()
			{
				return attributes;
			}

			const Uniforms<U...>& getUniforms()
			{
				return uniforms;
			}

			void upload(vk::Device& device, vk::PhysicalDevice& physicalDevice)
			{
				uniforms.upload(device, physicalDevice);
				attributes.upload(device, physicalDevice);
			}

			/**
			* \brief Updates the uniform buffers for this shader.
			* \param camera The camera to calculate the model view projection matrix from.
			* \param device The VulkanDevice to map and unmap memory from.
			*/
			virtual void updateUniformBuffers(Camera& camera, vk::Device& device) = 0;

		protected:
			Attributes<A...> attributes;
			Uniforms<U...> uniforms;
		};
	}
}
