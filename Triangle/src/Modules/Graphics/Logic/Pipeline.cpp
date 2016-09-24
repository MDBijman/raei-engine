#include "Modules/Graphics/Logic/Pipeline.h"
#include <vulkan/vulkan.h>

namespace Graphics
{
	Pipeline::Pipeline(VkPipeline pipeline, VulkanPipelineLayout pipelineLayout) : vk(pipeline), layout(pipelineLayout)
	{
	}
}