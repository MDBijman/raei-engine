#pragma once
#include "../../Core/VulkanContext.h"

namespace graphics
{
	namespace data
	{
		template<class T>
		class buffer
		{
			T t;

			void* mapped;
			vk::Buffer buffer_;
			vk::DeviceMemory memory;
			vk::DescriptorBufferInfo* descriptor;
			int binding_;

		public:
			buffer(T t, int binding, VulkanContext& context) : 
				t(t),
				binding_(binding)
			{
				// Vertex shader uniform buffer block
				vk::BufferCreateInfo bufferInfo;
				bufferInfo
					.setSize(sizeof t)
					.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);

				buffer_ = context.device.createBuffer(bufferInfo);

				auto allocInfo = context.getMemoryRequirements(buffer_, vk::MemoryPropertyFlagBits::eHostVisible 
					| vk::MemoryPropertyFlagBits::eHostCoherent);

				memory = context.device.allocateMemory(allocInfo);
				context.device.bindBufferMemory(buffer_, memory, 0);

				// Store information in the uniform's descriptor
				descriptor = new vk::DescriptorBufferInfo();
				descriptor->setBuffer(buffer_)
					.setOffset(0)
					.setRange(sizeof t);

				mapped = context.device.mapMemory(memory, 0, sizeof t);
				upload(context, t);
			}
			
			buffer(buffer&& other) : 
				t(std::move(other.t)), 
				buffer_(std::move(other.buffer_)),
				memory(std::move(other.memory)),
				binding_(other.binding_)
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
			}

			vk::WriteDescriptorSet getWriteDescriptorSet()
			{
				return vk::WriteDescriptorSet()
					.setDescriptorCount(1)
					.setDescriptorType(vk::DescriptorType::eUniformBuffer)
					.setPBufferInfo(descriptor)
					.setDstBinding(binding_);
			}
		};

		template<class T, int Binding, vk::ShaderStageFlagBits Stage>
		class buffer_template
		{
		public:
			using concrete_t = buffer<T>;

			static vk::DescriptorSetLayoutBinding getDescriptorSetLayoutBinding()
			{
				return vk::DescriptorSetLayoutBinding()
					.setDescriptorType(vk::DescriptorType::eUniformBuffer)
					.setDescriptorCount(1)
					.setBinding(Binding)
					.setStageFlags(Stage);
			}

			static vk::DescriptorPoolSize getDescriptorPoolSize()
			{
				return vk::DescriptorPoolSize()
					.setType(vk::DescriptorType::eUniformBuffer)
					.setDescriptorCount(1);
			}

			static concrete_t create(T t, VulkanContext& context)
			{
				return concrete_t(t, context);
			}
		};
	}
}