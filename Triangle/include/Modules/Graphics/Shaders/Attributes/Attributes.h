#pragma once
#include <tuple>
#include <vulkan/VULKAN.HPP>
#include "Modules/Graphics/Shaders/GPUBuffer.h"
#include "Modules/TemplateUtils/TemplatePackSize.h"
#include "Modules/TemplateUtils/OrderedTuple.h"

#include <optional>

namespace Graphics
{
	namespace Data
	{
		template<class... T>
		class Attributes
		{
		public:
			Attributes(std::vector<OrderedTuple<T...>>&& vertices, std::vector<uint32_t>&& indices) : vertices(std::move(vertices)), indices(std::move(indices))
			{
				initialize();
			}

			Attributes(std::vector<OrderedTuple<T...>>&& vertices) : vertices(std::move(vertices)) 
			{
				initialize();
			}

			Attributes(Attributes&& other) : vertices(std::move(other.vertices)), indices(std::move(other.indices)) {}

			void bind(vk::CommandBuffer& cmdBuffer)
			{
				if(isIndexed())
					cmdBuffer.bindIndexBuffer(indices.value().buffer, 0, vk::IndexType::eUint32);

				cmdBuffer.bindVertexBuffers(0, vertices.buffer, { 0 });
			}

			void allocate(VulkanContext& context)
			{
				vertices.allocate(context);
				vertices.upload(context);
				if(isIndexed())
				{
					indices.value().allocate(context);
					indices.value().upload(context);
				}
			}

			bool isIndexed()
			{
				return indices.has_value();
			}

			auto& getIndices()
			{
				return indices;
			}

			auto& getVertices()
			{
				return vertices;
			}

		private:
			void initialize()
			{
				vertices.bindingDescriptions = new std::vector<vk::VertexInputBindingDescription>();
				vertices.bindingDescriptions->push_back(vk::VertexInputBindingDescription()
					.setBinding(0)
					.setStride(packSize<T...>::value)
					.setInputRate(vk::VertexInputRate::eVertex));

				vertices.vi
					.setVertexBindingDescriptionCount(static_cast<uint32_t>(vertices.bindingDescriptions->size()))
					.setPVertexBindingDescriptions(vertices.bindingDescriptions->data());

				vertices.attributeDescriptions = new std::vector<vk::VertexInputAttributeDescription>(getAttributeDescriptions());
				vertices.vi
					.setVertexAttributeDescriptionCount(static_cast<uint32_t>(vertices.attributeDescriptions->size()))
					.setPVertexAttributeDescriptions(vertices.attributeDescriptions->data());
			}

			std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
			{
				return{
					T::getAttributeDescription()...
				};
			}

			// TODO upload code duplication

			struct Vertices
			{
				Vertices(std::vector<OrderedTuple<T...>>&& d) : data(d) {}

				void allocate(VulkanContext& context)
				{
					size_t dataSize = data.size() * sizeof(std::tuple<T...>);

					vk::BufferCreateInfo bufInfo;
					bufInfo
						.setSize(dataSize)
						.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);

					//	Copy data to VRAM
					buffer = context.device.createBuffer(bufInfo);
					vk::MemoryRequirements memReqs = context.device.getBufferMemoryRequirements(buffer);

					uint32_t memoryTypeIndex = -1;

					auto properties = context.physicalDevice.getMemoryProperties();
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

					memAlloc
						.setAllocationSize(memReqs.size)
						.setMemoryTypeIndex(memoryTypeIndex);

					memory = context.device.allocateMemory(memAlloc);
				}

				void upload(VulkanContext& context)
				{
					void *mappedMemory;
					mappedMemory = context.device.mapMemory(memory, 0, memAlloc.allocationSize);
					size_t dataSize = data.size() * sizeof(std::tuple<T...>);
					memcpy(mappedMemory, data.data(), dataSize);
					context.device.unmapMemory(memory);
					context.device.bindBufferMemory(buffer, memory, 0);
				}

				std::vector<vk::VertexInputBindingDescription>* bindingDescriptions;
				std::vector<vk::VertexInputAttributeDescription>* attributeDescriptions;

				vk::PipelineVertexInputStateCreateInfo vi;
				std::vector<OrderedTuple<T...>> data;

				vk::MemoryAllocateInfo memAlloc;
				vk::Buffer buffer;
				vk::DeviceMemory memory;
			} vertices;

			struct Indices
			{
				Indices(std::vector<uint32_t>&& i) : data(i) {}

				void allocate(VulkanContext& context)
				{
					vk::BufferCreateInfo indexBufferInfo;
					indexBufferInfo
						.setSize(static_cast<uint32_t>(data.size()) * sizeof(uint32_t))
						.setUsage(vk::BufferUsageFlagBits::eIndexBuffer);

					// Copy index data to VRAM
					buffer = context.device.createBuffer(indexBufferInfo);
					vk::MemoryRequirements memReqs = context.device.getBufferMemoryRequirements(buffer);

					uint32_t memoryTypeIndex = -1;

					auto properties = context.physicalDevice.getMemoryProperties();
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

					memAlloc
						.setAllocationSize(memReqs.size)
						.setMemoryTypeIndex(memoryTypeIndex);

					memory = context.device.allocateMemory(memAlloc);
				}

				void upload(VulkanContext& context)
				{
					void *mappedMemory;
					mappedMemory = context.device.mapMemory(memory, 0, data.size() * sizeof(uint32_t));
					memcpy(mappedMemory, data.data(), data.size() * sizeof(uint32_t));
					context.device.unmapMemory(memory);
					context.device.bindBufferMemory(buffer, memory, 0);
				}

				vk::MemoryAllocateInfo memAlloc;
				vk::Buffer buffer;
				vk::DeviceMemory memory;

				std::vector<uint32_t> data;
			};
			std::optional<Indices> indices;
		};


	}
}