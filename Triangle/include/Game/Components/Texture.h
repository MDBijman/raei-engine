#pragma once
#include "Modules/ECS/ECS.h"
#include "Modules/Graphics/Data/Shaders/Texture.h"

namespace Components
{
	class Texture : public Component
	{
	public:
		Texture(const std::string& name, Graphics::VulkanContext& context, vk::CommandPool& cmdPool, vk::Queue& queue) : texture(("./res/textures/" + name).c_str(), vk::Format::eBc3UnormBlock, context.physicalDevice, context.device, cmdPool, queue)
		{
		
		}

		Graphics::Data::Texture<0, vk::ShaderStageFlagBits::eFragment> texture;
	};
}
