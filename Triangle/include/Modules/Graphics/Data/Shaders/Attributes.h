#pragma once
#include <tuple>
#include <vulkan/VULKAN.HPP>
#include "Modules/Graphics/Data/GPUBuffer.h"
#include "Modules/TemplateUtils/TemplatePackSize.h"
#include "Modules/Graphics/Data/OrderedTuple.h"

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

			void upload(vk::Device& device, vk::PhysicalDevice& physicalDevice)
			{
				vertices.allocate(device, physicalDevice);
				vertices.upload(device);
				if(isIndexed())
				{
					indices.value().allocate(device, physicalDevice);
					indices.value().upload(device);
				}
			}

			void setVertices(std::vector<OrderedTuple<T...>>&& vertices)
			{
				assert(vertices.size() == this->vertices.size());
				this->vertices.data = vertices;
				this->vertices.upload();
			}

			void setIndices(std::vector<uint32_t>&& indices)
			{
				assert(indices.size() == this->indices.size());
				if(isIndexed())
				{
					this->indices.value().data = indices;
					this->indices.value().upload();
				}
			}

			bool isIndexed()
			{
				return indices.has_value();
			}

			const std::vector<uint32_t>& getIndices()
			{
				if(indices.has_value())
					return indices.value().data;
				return {};
			}

			const std::vector<OrderedTuple<T...>>& getData()
			{
				return vertices.data;
			}

			const vk::PipelineVertexInputStateCreateInfo& getVI()
			{
				return vertices.vi;
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

				void allocate(vk::Device& device, vk::PhysicalDevice& physicalDevice)
				{
					size_t dataSize = vertices.data.size() * sizeof(std::tuple<T...>);

					vk::BufferCreateInfo bufInfo;
					bufInfo
						.setSize(dataSize)
						.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);

					//	Copy data to VRAM
					buffer = device.createBuffer(bufInfo);
					vk::MemoryRequirements memReqs = device.getBufferMemoryRequirements(buffer);

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

					memory = device.allocateMemory(memAlloc);
				}

				void upload(vk::Device& device)
				{
					void *mappedMemory;
					mappedMemory = device.mapMemory(memory, 0, memAlloc.allocationSize);
					size_t dataSize = vertices.data.size() * sizeof(std::tuple<T...>);
					memcpy(mappedMemory, data.data(), dataSize);
					device.unmapMemory(memory);
					device.bindBufferMemory(buffer, memory, 0);
				}

				std::vector<vk::VertexInputBindingDescription>* bindingDescriptions;
				std::vector<vk::VertexInputAttributeDescription>* attributeDescriptions;

				vk::PipelineVertexInputStateCreateInfo vi;
				std::vector<OrderedTuple<T...>> data;

				vk::Buffer buffer;
				vk::DeviceMemory memory;
			} vertices;

			struct Indices
			{
				Indices(std::vector<uint32_t>&& i) : data(i) {}

				void allocate(vk::Device& device, vk::PhysicalDevice& physicalDevice)
				{
					vk::BufferCreateInfo indexBufferInfo;
					indexBufferInfo
						.setSize(static_cast<uint32_t>(data.size()) * sizeof(uint32_t))
						.setUsage(vk::BufferUsageFlagBits::eIndexBuffer);

					// Copy index data to VRAM
					buffer = device.createBuffer(indexBufferInfo);
					vk::MemoryRequirements memReqs = device.getBufferMemoryRequirements(buf);

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

					memory = device.allocateMemory(memAlloc);
				}

				void upload(vk::Device& device)
				{
					void *mappedMemory;
					mappedMemory = device.mapMemory(memory, 0, data.size() * sizeof(uint32_t));
					memcpy(mappedMemory, data.data(), data.size() * sizeof(uint32_t));
					device.unmapMemory(memory);
					device.bindBufferMemory(buffer, memory, 0);
					count = static_cast<uint32_t>(data.size());
				}

				vk::Buffer buffer;
				vk::DeviceMemory memory;

				std::vector<uint32_t> data;
			};
			std::optional<Indices> indices;
		};
	}
}