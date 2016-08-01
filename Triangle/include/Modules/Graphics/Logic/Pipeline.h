#pragma once
#include <vulkan\vulkan.h>

#include "Modules\Graphics\VulkanWrappers\VulkanWrappers.h"

namespace Graphics
{
	class Pipeline
	{
	public:
		Pipeline();
		Pipeline(VkPipeline pipeline);

		VkPipeline vk;
		VulkanPipelineLayout layout;
	};
}
