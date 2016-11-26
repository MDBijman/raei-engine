#pragma once
#include "Modules/Graphics/Data/Geometry/GPUBuffer.h"

namespace Graphics
{
	namespace Data
	{
		class Indices : public GPUBuffer
		{
		public:
			void setData(std::vector<uint32_t> indices)
			{
				this->indices = indices;
			}

			std::vector<uint32_t>& getData()
			{
				return this->indices;
			}

			void upload(vk::Device& device, vk::PhysicalDevice& physicalDevice) override
			{
				vk::BufferCreateInfo indexBufferInfo;
				indexBufferInfo
					.setSize(static_cast<uint32_t>(indices.size()) * sizeof(uint32_t))
					.setUsage(vk::BufferUsageFlagBits::eIndexBuffer);

				vk::MemoryRequirements memReqs;
				void *data;

				// Copy index data to VRAM
				buf = device.createBuffer(indexBufferInfo);
				memReqs = device.getBufferMemoryRequirements(buf);

				uint32_t memoryTypeIndex = -1;

				auto properties = physicalDevice.getMemoryProperties();
				for(uint32_t i = 0; i < 32; i++)
				{
					if(memReqs.memoryTypeBits & 1)
					{
						if(properties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible)
						{
							memoryTypeIndex = i;
							break;
						}
					}
					memReqs.memoryTypeBits >>= 1;
				}

				vk::MemoryAllocateInfo memAlloc;
				memAlloc
					.setAllocationSize(memReqs.size)
					.setMemoryTypeIndex(memoryTypeIndex);

				mem = device.allocateMemory(memAlloc);
				data = device.mapMemory(mem, 0, indices.size() * sizeof(uint32_t));
				memcpy(data, indices.data(), indices.size() * sizeof(uint32_t));
				device.unmapMemory(mem);
				device.bindBufferMemory(buf, mem, 0);
				count = static_cast<uint32_t>(indices.size());
			}

			vk::Buffer& getBuffer()
			{
				return buf;
			}

			int count;

		private:
			std::vector<uint32_t> indices;

			vk::Buffer buf;
			vk::DeviceMemory mem;
		};
	}
}
