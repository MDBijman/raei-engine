#pragma once
#include "Modules/Graphics/Logic/Pipeline.h"
#include "Modules/Importers/JSON.h"

#include <vector>

namespace Importers
{
	namespace Pipeline 
	{
		Graphics::Pipeline load(const std::string& location, vk::PipelineLayout& layout, vk::PipelineVertexInputStateCreateInfo& vi, vk::RenderPass& rp, vk::PipelineCache& cache, vk::Device& device);

		vk::PipelineInputAssemblyStateCreateInfo parseInputAssemblyState(JSON::JSON& json);
		vk::PipelineRasterizationStateCreateInfo parseRasterizationState(JSON::JSON& json);
		vk::PipelineColorBlendStateCreateInfo parseBlendAttachmentState(JSON::JSON& json, Graphics::Pipeline& pipeline);
		vk::PipelineViewportStateCreateInfo parseViewportState(JSON::JSON& json);
		vk::PipelineDynamicStateCreateInfo parseDynamicStates(JSON::JSON& json, Graphics::Pipeline& pipeline);
		vk::PipelineDepthStencilStateCreateInfo parseDepthAndStencilState(JSON::JSON & json);
		vk::PipelineMultisampleStateCreateInfo parseMultisamplingState(JSON::JSON & json);
		std::vector<vk::PipelineShaderStageCreateInfo> parseShaderStages(JSON::JSON & json, vk::Device& device);
	};
}