#pragma once

namespace Graphics
{
	namespace Data
	{
		template<int BINDING, vk::ShaderStageFlagBits STAGE>
		class UniformBuffer
		{
		public:
			void upload(vk::Device& device, vk::PhysicalDevice& physicalDevice) 
			{
			}

			vk::WriteDescriptorSet getWriteDescriptorSet()
			{
				return vk::WriteDescriptorSet()
					.setDescriptorCount(1)
					.setDescriptorType(vk::DescriptorType::eUniformBuffer)
					.setPBufferInfo(descriptor)
					.setDstBinding(BINDING);
			}

			static vk::DescriptorSetLayoutBinding getDescriptorSetLayoutBinding()
			{
				return vk::DescriptorSetLayoutBinding()
					.setDescriptorType(vk::DescriptorType::eUniformBuffer)
					.setDescriptorCount(1)
					.setBinding(BINDING)
					.setStageFlags(STAGE);
			}

			static vk::DescriptorPoolSize getDescriptorPoolSize()
			{
				return vk::DescriptorPoolSize()
					.setType(vk::DescriptorType::eUniformBuffer)
					.setDescriptorCount(1);
			}

			vk::Buffer buffer;
			vk::DeviceMemory memory;
			vk::DescriptorBufferInfo descriptor;
		};
	}
}