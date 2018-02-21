#pragma once
#include <vulkan/vulkan.hpp>

#include "VulkanContext.h"

namespace graphics
{
	namespace gpu
	{
		template<class T>
		class buffer
		{
			static constexpr size_t item_size = sizeof(T);

		protected:
			std::vector<T> data_;
			size_t data_size;

			vk::MemoryAllocateInfo memAlloc;
			vk::Buffer buffer_;
			vk::DeviceMemory memory;
			vk::BufferUsageFlagBits usage_bits_;

		public:
			buffer(vk::BufferUsageFlagBits usage_flag_bits, std::vector<T> data) :
				data_(data), data_size(item_size * data_.size()), usage_bits_(usage_flag_bits) {}

			buffer(buffer&& o) :
				data_(std::move(o.data_)),
				data_size(std::move(o.data_size)),
				memAlloc(std::move(o.memAlloc)),
				buffer_(std::move(o.buffer_)),
				memory(std::move(o.memory)),
				usage_bits_(std::move(o.usage_bits_))
			{}

			virtual ~buffer() {}

			void allocate(VulkanContext& context)
			{
				vk::BufferCreateInfo bufInfo;
				bufInfo
					.setSize(data_size)
					.setUsage(usage_bits_);

				//	Copy data to VRAM
				buffer_ = context.device.createBuffer(bufInfo);
				vk::MemoryRequirements memReqs = context.device.getBufferMemoryRequirements(buffer_);

				uint32_t memoryTypeIndex = -1;

				auto properties = context.physicalDevice.getMemoryProperties();
				for (uint32_t i = 0; i < 32; i++)
				{
					if (memReqs.memoryTypeBits & 1)
					{
						if (properties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible)
						{
							memoryTypeIndex = i;
							break;
						}
					}
					memReqs.memoryTypeBits >>= 1;
				}

				memAlloc
					.setAllocationSize(memReqs.size)
					.setMemoryTypeIndex(memoryTypeIndex);

				memory = context.device.allocateMemory(memAlloc);
				context.device.bindBufferMemory(buffer_, memory, 0);
			}

			void upload(VulkanContext& context)
			{
				void *mappedMemory;
				mappedMemory = context.device.mapMemory(memory, 0, memAlloc.allocationSize);
				memcpy(mappedMemory, data_.data(), data_size);
				context.device.unmapMemory(memory);
			}

			std::vector<T>& data()
			{
				return data_;
			}

			vk::Buffer& vk_buffer()
			{
				return buffer_;
			}
		};
	}
}
