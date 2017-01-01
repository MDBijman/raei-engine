#pragma once

namespace Graphics
{
	namespace Data
	{
		template<class T, int BINDING, vk::ShaderStageFlagBits STAGE>
		class UniformBuffer
		{
		public:
			UniformBuffer(T t, VulkanContext& context) : t(t)
			{
				// Vertex shader uniform buffer block
				vk::BufferCreateInfo bufferInfo;
				bufferInfo
					.setSize(sizeof t)
					.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);

				buffer = context.device.createBuffer(bufferInfo);

				auto allocInfo = context.getMemoryRequirements(buffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

				memory = context.device.allocateMemory(allocInfo);
				context.device.bindBufferMemory(buffer, memory, 0);

				// Store information in the uniform's descriptor
				descriptor = new vk::DescriptorBufferInfo();
				descriptor->setBuffer(buffer)
					.setOffset(0)
					.setRange(sizeof t);

				mapped = context.device.mapMemory(memory, 0, sizeof t);
				upload(context, t);
			}
			
			UniformBuffer(UniformBuffer&& other) : t(std::move(other.t)), buffer(std::move(other.buffer)), memory(std::move(other.memory))
			{
				this->descriptor = other.descriptor;
				other.descriptor = nullptr;
				this->mapped = other.mapped;
				other.mapped = nullptr;
			}
			
			void allocate(VulkanContext& context)
			{

			}

			void upload(VulkanContext& context, T& t)
			{
				memcpy(mapped, &t, sizeof t);
				//context.device.unmapMemory(memory);
				//mapped = nullptr;
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

			void* mapped;
			vk::Buffer buffer;
			vk::DeviceMemory memory;
			vk::DescriptorBufferInfo* descriptor;
		};
	}
}