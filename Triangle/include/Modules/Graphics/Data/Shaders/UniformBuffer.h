#pragma once

namespace Graphics
{
	namespace Data
	{
		template<class T, int BINDING, vk::ShaderStageFlagBits STAGE>
		class UniformBuffer
		{
		public:
			UniformBuffer(T t) : t(t) {}
			
			
			void allocate(VulkanContext& context)
			{
				// Vertex shader uniform buffer block
				vk::BufferCreateInfo bufferInfo;
				bufferInfo
					.setSize(sizeof t)
					.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);

				buffer = context.device.createBuffer(bufferInfo);

				auto allocInfo = context.getMemoryRequirements(buffer, vk::MemoryPropertyFlagBits::eHostVisible);

				memory = context.device.allocateMemory(allocInfo);
				context.device.bindBufferMemory(buffer, memory, 0);

				// Store information in the uniform's descriptor
				descriptor.buffer = buffer;
				descriptor.offset = 0;
				descriptor.range = sizeof t;
			}

			void upload(VulkanContext& context, T& t)
			{
				// Map uniform buffer and update it
				void *pData;
				pData = device.mapMemory(memory, 0, sizeof t);
				memcpy(pData, &t, sizeof t);
				device.unmapMemory(memory);
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

			T t;

			vk::Buffer buffer;
			vk::DeviceMemory memory;
			vk::DescriptorBufferInfo descriptor;
		};
	}
}