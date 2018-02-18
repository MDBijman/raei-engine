#pragma once
#include "Modules/Graphics/Shaders/Uniforms/Texture.h"
#include "Modules/Graphics/VulkanWrappers/VulkanContext.h"
#include "Modules/ECS/Component.h"

#include <vulkan/VULKAN.HPP>

namespace Components
{
	class Texture : public ecs::Component
	{
	public:
		Texture(const std::string& name, graphics::VulkanContext& context, vk::CommandPool& cmdPool, vk::Queue& queue) : texture(("./res/textures/" + name).c_str(), vk::Format::eBc3UnormBlock, context.physicalDevice, context.device, cmdPool, queue)
		{
		
		}

		graphics::data::Texture<0, vk::ShaderStageFlagBits::eFragment> texture;
	};
}
