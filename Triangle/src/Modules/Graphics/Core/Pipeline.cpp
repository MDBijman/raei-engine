#include "stdafx.h"
#include "Modules/Graphics/Core/Pipeline.h"

namespace graphics
{
	Pipeline::Pipeline(vk::Pipeline pipeline, vk::PipelineLayout pipelineLayout) : vk(pipeline), layout(pipelineLayout)
	{
	}
}