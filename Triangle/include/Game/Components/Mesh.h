#pragma once
#include "Modules/ECS/ECS.h"
#include "Modules/Importers/Obj.h"

namespace Components
{
	class Mesh : public Component
	{
	public:
		Mesh(const std::string& name, VulkanDevice& device, VulkanPhysicalDevice& physicalDevice)
		{
			Graphics::Data::Mesh* m = Importers::Obj::load(("./res/models/" + name));

			mesh.vertices.setData(m->vertices.getData());
			mesh.vertices.upload(device, physicalDevice);

			mesh.indices.setData(m->indices.getData());
			mesh.indices.upload(device, physicalDevice);


			std::vector<VulkanVertexInputBindingDescription> bindingDescriptions(1);
			// Binding description
			bindingDescriptions[0]
				.setBinding(VERTEX_BUFFER_BIND_ID)
				.setStride(sizeof(Vertex))
				.setInputRate(VK_VERTEX_INPUT_RATE_VERTEX);

			mesh.vertices.vi.setVertexBindingDescriptions(bindingDescriptions);

			std::vector<VulkanVertexInputAttributeDescription> attributeDescriptions(3);
			// Attribute descriptions
			// Describes memory layout and shader attribute locations
			// Location 0 : Position
			attributeDescriptions[0]
				.setBinding(VERTEX_BUFFER_BIND_ID)
				.setOffset(0)
				.setLocation(0)
				.setFormat(VK_FORMAT_R32G32B32_SFLOAT);

			// Location 1 : UV
			attributeDescriptions[1]
				.setBinding(VERTEX_BUFFER_BIND_ID)
				.setLocation(1)
				.setFormat(VK_FORMAT_R32G32B32_SFLOAT)
				.setOffset(sizeof(float) * 3);

			// Location 2 : Normal
			attributeDescriptions[2]
				.setBinding(VERTEX_BUFFER_BIND_ID)
				.setLocation(2)
				.setFormat(VK_FORMAT_R32G32B32_SFLOAT)
				.setOffset(sizeof(float) * 5);

			// Assign to vertex buffer
			mesh.vertices.vi.setVertexAttributeDescriptions(attributeDescriptions);
		}

		Graphics::Data::Mesh mesh;

	};
}
