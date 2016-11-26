#pragma once
#include "Modules/ECS/ECS.h"
#include "Modules/Importers/Obj.h"

#define VERTEX_BUFFER_BIND_ID 0

namespace Components
{
	class Mesh : public Component
	{
	public:
		Mesh(const std::string& name, vk::Device& device, vk::PhysicalDevice& physicalDevice)
		{
			bindingDescriptions = new std::vector<vk::VertexInputBindingDescription>();
			attributeDescriptions = new std::vector<vk::VertexInputAttributeDescription>();

			mesh = Importers::Obj::load(("./res/models/" + name));

			mesh->vertices.upload(device, physicalDevice);
			mesh->indices.upload(device, physicalDevice);

			// Binding description
			bindingDescriptions->resize(1);
			bindingDescriptions->at(0)
				.setBinding(VERTEX_BUFFER_BIND_ID)
				.setStride(sizeof(Vertex))
				.setInputRate(vk::VertexInputRate::eVertex);

			mesh->vertices.vi
				.setVertexBindingDescriptionCount(1)
				.setPVertexBindingDescriptions(bindingDescriptions->data());

			// Attribute descriptions
			// Describes memory layout and shader attribute locations
			// Location 0 : Position
			attributeDescriptions->resize(3);
			attributeDescriptions->at(0)
				.setBinding(VERTEX_BUFFER_BIND_ID)
				.setOffset(0)
				.setLocation(0)
				.setFormat(vk::Format::eR32G32B32Sfloat);

			// Location 1 : UV
			attributeDescriptions->at(1)
				.setBinding(VERTEX_BUFFER_BIND_ID)
				.setLocation(1)
				.setFormat(vk::Format::eR32G32B32Sfloat)
				.setOffset(sizeof(float) * 3);

			// Location 2 : Normal
			attributeDescriptions->at(2)
				.setBinding(VERTEX_BUFFER_BIND_ID)
				.setLocation(2)
				.setFormat(vk::Format::eR32G32B32Sfloat)
				.setOffset(sizeof(float) * 5);

			// Assign to vertex buffer
			mesh->vertices.vi
				.setVertexAttributeDescriptionCount(3)
				.setPVertexAttributeDescriptions(attributeDescriptions->data());
		}

		Mesh(Mesh&& other) : Component(other)
		{
			this->bindingDescriptions = other.bindingDescriptions;
			other.bindingDescriptions = nullptr;

			this->attributeDescriptions	= other.attributeDescriptions;
			other.attributeDescriptions = nullptr;

			this->mesh = other.mesh;
			other.mesh = nullptr;
		}

		Graphics::Data::Mesh* mesh;

	private:
		std::vector<vk::VertexInputBindingDescription>* bindingDescriptions;
		std::vector<vk::VertexInputAttributeDescription>* attributeDescriptions;
	};
}
