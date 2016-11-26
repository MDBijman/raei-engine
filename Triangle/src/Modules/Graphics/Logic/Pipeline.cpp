#include "Modules/Graphics/Logic/Pipeline.h"

namespace Graphics
{
	Pipeline::Pipeline(vk::Pipeline pipeline, vk::PipelineLayout pipelineLayout) : vk(pipeline), layout(pipelineLayout)
	{
	}
}