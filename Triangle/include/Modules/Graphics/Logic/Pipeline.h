#pragma once
#include <vulkan/vulkan.h>

#include "Modules/Graphics/VulkanWrappers/VulkanWrappers.h"

namespace Graphics
{
	class Pipeline
	{
	public:
		Pipeline() : vk(VK_NULL_HANDLE) {}
		Pipeline(VkPipeline pipeline, VulkanPipelineLayout pipelineLayout);

		VkPipeline vk;
		VulkanPipelineLayout layout;
	};
}
