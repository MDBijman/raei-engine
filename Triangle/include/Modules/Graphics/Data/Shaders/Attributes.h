#pragma once
#include <tuple>
#include <vulkan/VULKAN.HPP>
#include "Modules/Graphics/Data/GPUBuffer.h"
#include "Modules/TemplateUtils/TemplatePackSize.h"

namespace Graphics
{
	namespace Data
	{
		template<class... T>
		class Attributes : public GPUBuffer
		{
		public:
			Attributes(std::vector<std::tuple<T...>> data) : data(data)
			{
				bindingDescriptions = new std::vector<vk::VertexInputBindingDescription>();
				bindingDescriptions->push_back(vk::VertexInputBindingDescription()
					.setBinding(0)
					.setStride(packSize<T...>::value)
					.setInputRate(vk::VertexInputRate::eVertex));

				vi
					.setVertexBindingDescriptionCount(static_cast<uint32_t>(bindingDescriptions->size()))
					.setPVertexBindingDescriptions(bindingDescriptions->data());

				attributeDescriptions = new std::vector<vk::VertexInputAttributeDescription>(getAttributeDescriptions<T...>());
				vi
					.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions->size()))
					.setPVertexAttributeDescriptions(attributeDescriptions->data());
			}

			Attributes(Attributes&& other)
			{
				vi = other.vi;
				this->bindingDescriptions = other.bindingDescriptions;
				other.bindingDescriptions = nullptr;
				this->attributeDescriptions = other.attributeDescriptions;
				other.attributeDescriptions = nullptr;
				this->data = other.data;
			}

			void upload(vk::Device& device, vk::PhysicalDevice& physicalDevice) override
			{
				vk::MemoryRequirements memReqs;
				void *mappedMemory;

				size_t dataSize = data.size() * sizeof(std::tuple<T...>);

				vk::BufferCreateInfo bufInfo;
				bufInfo
					.setSize(dataSize)
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
				mappedMemory = device.mapMemory(mem, 0, memAlloc.allocationSize);
				memcpy(mappedMemory, data.data(), dataSize);
				device.unmapMemory(mem);
				device.bindBufferMemory(buf, mem, 0);
			}

			vk::PipelineVertexInputStateCreateInfo& getVI()
			{
				return vi;
			}

		private:
			std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
			{
				return getAttributeDescriptions<T...>();
			}

			template<class... T>
			std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
			{
				return{
					T::getAttributeDescription()...
				};
			}

			std::vector<vk::VertexInputBindingDescription>* bindingDescriptions;
			std::vector<vk::VertexInputAttributeDescription>* attributeDescriptions;

			vk::PipelineVertexInputStateCreateInfo vi;
			
			std::vector<std::tuple<T...>> data;
		};
	}
}