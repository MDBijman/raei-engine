#pragma once
#include "Modules/ECS/ECS.h"
#include "Modules/Graphics/Data/Textures/Texture.h"

namespace Components
{
	class Texture : public Component
	{
	public:
		Texture(const std::string& name, vk::Device& device, vk::PhysicalDevice& physicalDevice, vk::CommandPool& cmdPool, vk::Queue& queue)
		{
			texture.load(("./res/textures/" + name).c_str(), vk::Format::eBc3UnormBlock, physicalDevice, device, cmdPool, queue);
		}

		Graphics::Data::Texture texture;
	};
}
