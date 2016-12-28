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

			Attributes<A...>& getAttributes()
			{
				return attributes;
			}

			Uniforms<U...>& getUniforms()
			{
				return uniforms;
			}

			void upload(vk::Device& device, vk::PhysicalDevice& physicalDevice)
			{
				uniforms.upload(device, physicalDevice);
				attributes.upload(device, physicalDevice);
			}

			void draw(vk::CommandBuffer& cmdBuffer)
			{
				attributes.bind(cmdBuffer);
				if(attributes.isIndexed())
					cmdBuffer.drawIndexed(attributes.getIndices().size(), 1, 0, 0, 0);
				else
					cmdBuffer.draw(attributes.getData().size(), 1, 0, 0);
			}

		protected:
			Attributes<A...> attributes;
			Uniforms<U...> uniforms;
		};
	}
}
