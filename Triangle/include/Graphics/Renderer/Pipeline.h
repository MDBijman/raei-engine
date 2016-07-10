#pragma once
#include <vulkan\vulkan.h>

namespace Graphics
{
	class Pipeline
	{
	public:
		Pipeline();
		Pipeline(VkPipeline pipeline);

		VkPipeline vk;
	};
}
