#include "stdafx.h"
#include "Modules/Graphics/Logic/Pipeline.h"

namespace graphics
{
	Pipeline::Pipeline(vk::Pipeline pipeline, vk::PipelineLayout pipelineLayout) : vk(pipeline), layout(pipelineLayout)
	{
	}
}