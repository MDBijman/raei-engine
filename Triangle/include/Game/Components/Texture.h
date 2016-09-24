#pragma once
#include "Modules/ECS/ECS.h"
#include "Modules/Graphics/Data/Textures/Texture.h"

namespace Components
{
	class Texture : public Component
	{
	public:
		Texture(const std::string& name, VulkanDevice& device, VulkanPhysicalDevice& physicalDevice, VkCommandPool& cmdPool, VulkanQueue& queue)
		{
			texture.load(("./res/textures/" + name).c_str(), VK_FORMAT_BC3_UNORM_BLOCK, physicalDevice, device, cmdPool, queue);
		}

		Graphics::Data::Texture texture;
	};
}
