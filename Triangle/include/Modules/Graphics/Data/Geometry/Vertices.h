#pragma once
#include "Modules\Graphics\VulkanWrappers\VulkanWrappers.h"
#include "Modules\Graphics\Data\Geometry\GPUBuffer.h"

#include <vector>
#include <glm\glm.hpp>

struct Vertex
{
	Vertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal) : pos(pos), uv(uv), normal(normal) {}
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 normal;
};

namespace Graphics
{
	namespace Data
	{
		class Vertices : public GPUBuffer
		{
		public:
			void setData(std::vector<Vertex> vertices)
			{
				this->vertices = vertices;
			}
			
			std::vector<Vertex>& getData()
			{
				return vertices;
			}

			void upload(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice)
			{
				VkMemoryRequirements memReqs;
				void *data;

				VulkanBufferCreateInfo bufInfo;
				bufInfo
					.setSize(vertices.size() * sizeof(Vertex))
					.setUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
					.setFlags(0);

				//	Copy data to VRAM
				buf = device.createBuffer(bufInfo);
				memReqs = device.getBufferMemoryRequirements(buf);

				VulkanMemoryAllocateInfo memAlloc;
				memAlloc
					.setAllocationSize(memReqs.size)
					.setMemoryTypeIndex(physicalDevice.getMemoryPropertyIndex(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memReqs));

				memReqs.memoryTypeBits = 1;
				mem = device.allocateMemory(memAlloc.vkInfo);
				data = device.mapMemory(mem, memAlloc.vkInfo.allocationSize);
				memcpy(data, vertices.data(), vertices.size() * sizeof(Vertex));
				device.unmapMemory(mem);
				device.bindBufferMemory(buf, mem);
			}

			VkBuffer& getBuffer()
			{
				return buf;
			}

			VulkanPipelineVertexInputStateCreateInfo vi;

		private:
			std::vector<Vertex> vertices;

			VkBuffer buf;
			VkDeviceMemory mem;
		};
	}

}
