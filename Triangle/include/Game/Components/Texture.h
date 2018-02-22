#pragma once
#include "Modules/Graphics/Shaders/Uniforms/Texture.h"
#include "Modules/Graphics/Core/VulkanContext.h"
#include "Modules/ECS/Component.h"

#include <vulkan/VULKAN.HPP>

namespace Components
{
	class Texture : public ecs::Component
	{
	public:
		Texture(const std::string& name, graphics::VulkanContext& context, vk::CommandPool& cmdPool, vk::Queue& queue) :
			texture("./res/textures/" + name, 0, vk::ShaderStageFlagBits::eFragment, vk::Format::eBc3UnormBlock, 
				context.physicalDevice, context.device, cmdPool, queue)
		{
		
		}

		graphics::data::texture texture;
	};
}
