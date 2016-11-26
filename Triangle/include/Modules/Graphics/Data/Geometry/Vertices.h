#pragma once
#include "Modules/Graphics/Data/Geometry/GPUBuffer.h"

#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

struct Vertex
{
	Vertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal) : pos(pos), uv(uv), normal(normal)
	{}
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

			void upload(vk::Device& device, vk::PhysicalDevice& physicalDevice) override
			{
				vk::MemoryRequirements memReqs;
				void *data;

				vk::BufferCreateInfo bufInfo;
				bufInfo
					.setSize(static_cast<uint32_t>(vertices.size()) * sizeof(Vertex))
					.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);

				//	Copy data to VRAM
				buf = device.createBuffer(bufInfo);
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

				memReqs.memoryTypeBits = 1;
				mem = device.allocateMemory(memAlloc);
				data = device.mapMemory(mem, 0, memAlloc.allocationSize);
				memcpy(data, vertices.data(), vertices.size() * sizeof(Vertex));
				device.unmapMemory(mem);
				device.bindBufferMemory(buf, mem, 0);
			}

			vk::Buffer& getBuffer()
			{
				return buf;
			}

			vk::PipelineVertexInputStateCreateInfo vi;

		private:
			std::vector<Vertex> vertices;

			vk::Buffer buf;
			vk::DeviceMemory mem;
		};
	}

}
