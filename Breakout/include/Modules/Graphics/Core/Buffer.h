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
			vk::MemoryAllocateInfo memAlloc;
			vk::Buffer buffer_;
			vk::DeviceMemory memory;
			vk::BufferUsageFlagBits usage_bits_;
			VulkanContext& context;
			size_t data_size;

		public:
			buffer(VulkanContext& ctx, vk::BufferUsageFlagBits usage_flag_bits, std::vector<T> data) :
				usage_bits_(usage_flag_bits), context(ctx), data_size(item_size * data.size())
			{
				if (data_size > 0)
				{
					allocate();
					upload(data);
				}
			}

			buffer(buffer&& o) :
				memAlloc(std::move(o.memAlloc)),
				buffer_(std::move(o.buffer_)),
				memory(std::move(o.memory)),
				usage_bits_(std::move(o.usage_bits_)),
				context(o.context),
				data_size(o.data_size)
			{
				o.memAlloc = VK_NULL_HANDLE;
				o.buffer_ = nullptr;
				o.memory = nullptr;
				o.usage_bits_ = vk::BufferUsageFlagBits(0);
				o.data_size = 0;
			}

			virtual ~buffer()
			{
				deallocate();
			}

			void set(const std::vector<T>& data)
			{
				assert(data.size() > 0);

				// Reallocate
				if (data.size() * item_size != data_size)
				{
					data_size = item_size * data.size();
					deallocate();
					allocate();
				}

				upload(data);
			}

			vk::Buffer vk_buffer()
			{
				return this->buffer_;
			}

			size_t size()
			{
				return this->data_size;
			}

			size_t count()
			{
				return this->data_size / item_size;
			}

		private:
			void deallocate()
			{
				if (this->buffer_)
					context.device.destroyBuffer(this->buffer_);
				if (this->memory)
					context.device.freeMemory(this->memory);
			}

			void allocate()
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

			void upload(const std::vector<T>& data)
			{
				void* device_memory = context.device.mapMemory(memory, 0, memAlloc.allocationSize);
				memcpy(device_memory, data.data(), data_size);
				context.device.unmapMemory(memory);
			}
		};
	}
}
