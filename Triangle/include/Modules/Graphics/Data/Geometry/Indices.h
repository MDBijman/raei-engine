#pragma once
#include "Modules/Graphics/VulkanWrappers/VulkanWrappers.h"
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

			void upload(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice) override
			{
				VulkanBufferCreateInfo indexBufferInfo;
				indexBufferInfo
					.setSize(static_cast<uint32_t>(indices.size()) * sizeof(uint32_t))
					.setUsage(VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
					.setFlags(0);

				VkMemoryRequirements memReqs;
				void *data;

				// Copy index data to VRAM
				buf = device.createBuffer(indexBufferInfo);
				memReqs = device.getBufferMemoryRequirements(buf);

				VulkanMemoryAllocateInfo memAlloc;
				memAlloc
					.setAllocationSize(memReqs.size)
					.setMemoryTypeIndex(physicalDevice.getMemoryPropertyIndex(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memReqs));

				mem = device.allocateMemory(memAlloc.vkInfo);
				data = device.mapMemory(mem, indices.size() * sizeof(uint32_t));
				memcpy(data, indices.data(), indices.size() * sizeof(uint32_t));
				device.unmapMemory(mem);
				device.bindBufferMemory(buf, mem);
				count = static_cast<uint32_t>(indices.size());
			}

			VkBuffer& getBuffer()
			{
				return buf;
			}

			int count;

		private:
			std::vector<uint32_t> indices;

			VkBuffer buf;
			VkDeviceMemory mem;
		};
	}
}
