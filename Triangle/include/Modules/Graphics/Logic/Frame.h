#pragma once
#include <vulkan/VULKAN.HPP>
#include "../VulkanWrappers/VulkanContext.h"
#include "Renderer.h"

namespace Graphics
{
	class Frame
	{
		friend class Renderer;

	public:
		Frame(VulkanContext* context) : context(context) {}

		Frame& addCommandBuffer(vk::CommandBuffer buffer)
		{
			buffers.push_back(buffer);
			return *this;
		}

	private:
		VulkanContext* context;
		std::vector<vk::CommandBuffer> buffers;
	};
}