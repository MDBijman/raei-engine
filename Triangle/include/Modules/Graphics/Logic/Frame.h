#pragma once
#include "../VulkanWrappers/VulkanContext.h"
#include "Renderer.h"
#include <vector>

namespace graphics
{
	class Frame
	{
		friend class Renderer;

	public:
		Frame& addCommandBuffer(vk::CommandBuffer* buffer)
		{
			buffers.push_back(buffer);
			return *this;
		}

	private:
		std::vector<vk::CommandBuffer*> buffers;
	};
}