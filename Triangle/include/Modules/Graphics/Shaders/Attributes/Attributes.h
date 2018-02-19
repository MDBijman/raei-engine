#pragma once
#include "Modules/Graphics/Shaders/GPUBuffer.h"
#include "Modules/TemplateUtils/TemplatePackSize.h"
#include "Modules/TemplateUtils/OrderedTuple.h"
#include "Modules/Graphics/VulkanWrappers/VulkanContext.h"

#include <optional>

namespace graphics
{
	namespace data
	{
		template<class T>
		struct unpack;
		template<class... Ts>
		struct unpack<OrderedTuple<Ts...>> 
		{
			static std::vector<vk::VertexInputAttributeDescription> attribute_descriptions()
			{
				return{
					Ts::getAttributeDescription()...
				};
			}
		};

		template<class VertexData>
		class Vertices
		{
		private:
			static constexpr size_t vertex_size = sizeof(VertexData);
			std::size_t data_size;

		public:
			Vertices(std::vector<VertexData>&& d) : data(d), data_size(vertex_size * d.size()),
				bindingDescriptions(std::make_unique<std::vector<vk::VertexInputBindingDescription>>()),
				attributeDescriptions(std::make_unique<std::vector<vk::VertexInputAttributeDescription>>(
					unpack<VertexData>::attribute_descriptions()))
			{
				bindingDescriptions->push_back(vk::VertexInputBindingDescription()
					.setBinding(0)
					.setStride(vertex_size)
					.setInputRate(vk::VertexInputRate::eVertex));

				vi
					.setVertexBindingDescriptionCount(static_cast<uint32_t>(bindingDescriptions->size()))
					.setPVertexBindingDescriptions(bindingDescriptions->data());
				vi
					.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions->size()))
					.setPVertexAttributeDescriptions(attributeDescriptions->data());
			}

			void allocate(VulkanContext& context)
			{
				vk::BufferCreateInfo bufInfo;
				bufInfo
					.setSize(data_size)
					.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);

				//	Copy data to VRAM
				buffer = context.device.createBuffer(bufInfo);
				vk::MemoryRequirements memReqs = context.device.getBufferMemoryRequirements(buffer);

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
			}

			void upload(VulkanContext& context)
			{
				void *mappedMemory;
				mappedMemory = context.device.mapMemory(memory, 0, memAlloc.allocationSize);
				memcpy(mappedMemory, data.data(), data_size);
				context.device.unmapMemory(memory);
				context.device.bindBufferMemory(buffer, memory, 0);
			}

			std::unique_ptr<std::vector<vk::VertexInputBindingDescription>> bindingDescriptions;
			std::unique_ptr<std::vector<vk::VertexInputAttributeDescription>> attributeDescriptions;

			vk::PipelineVertexInputStateCreateInfo vi;
			std::vector<VertexData> data;

			vk::MemoryAllocateInfo memAlloc;
			vk::Buffer buffer;
			vk::DeviceMemory memory;
		};

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

		template<class... T>
		class Attributes
		{
		public:
			Attributes(std::vector<OrderedTuple<T...>>&& verts, std::vector<uint32_t>&& indices) :
				vertices(std::move(verts)), indices(std::move(indices)) {}

			Attributes(std::vector<OrderedTuple<T...>>&& vertices) : Attributes(std::move(vertices),
				std::vector<uint32_t>()) {}

			Attributes(Attributes&& other) : vertices(std::move(other.vertices)), indices(std::move(other.indices)) {}

			void allocate(VulkanContext& context)
			{
				vertices.allocate(context);
				vertices.upload(context);

				if (is_indexed())
				{
					indices.value().allocate(context);
					indices.value().upload(context);
				}
			}

			bool is_indexed()
			{
				return indices.has_value();
			}

			std::optional<Indices>& index_data()
			{
				return indices;
			}

			Vertices<OrderedTuple<T...>>& vertex_data()
			{
				return vertices;
			}

		private:
			Vertices<OrderedTuple<T...>> vertices;
			std::optional<Indices> indices;
		};
	}
}