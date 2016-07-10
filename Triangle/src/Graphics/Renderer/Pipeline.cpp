#include "Graphics\Renderer\Pipeline.h"
#include <vulkan\vulkan.h>

namespace Graphics
{
	Pipeline::Pipeline()
	{
		vk = VK_NULL_HANDLE;
	}

	Pipeline::Pipeline(VkPipeline pipeline) : vk(pipeline)
	{

	}
}