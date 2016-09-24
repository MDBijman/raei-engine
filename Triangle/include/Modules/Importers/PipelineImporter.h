#pragma once
#include "Modules/Graphics/VulkanWrappers/VulkanWrappers.h"
#include "Modules/Graphics/Logic/Pipeline.h"
#include "Modules/Importers/JSON.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Importers
{
	namespace Pipeline 
	{
		Graphics::Pipeline load(const std::string& location, VulkanPipelineLayout& layout, VulkanPipelineVertexInputStateCreateInfo& vi, VulkanRenderPass& rp, VkPipelineCache& cache, VulkanDevice& device);

		VulkanPipelineInputAssemblyStateCreateInfo parseInputAssemblyState(JSON::JSON& json);
		VulkanPipelineRasterizationStateCreateInfo parseRasterizationState(JSON::JSON& json);
		VulkanPipelineColorBlendStateCreateInfo parseBlendAttachmentState(JSON::JSON& json);
		VulkanPipelineViewportStateCreateInfo parseViewportState(JSON::JSON& json);
		VulkanPipelineDynamicStateCreateInfo parseDynamicStates(JSON::JSON& json);
		VulkanPipelineDepthStencilStateCreateInfo parseDepthAndStencilState(JSON::JSON & json);
		VulkanPipelineMultisampleStateCreateInfo parseMultisamplingState(JSON::JSON & json);
		std::vector<VulkanPipelineShaderStageCreateInfo> parseShaderStages(JSON::JSON & json, VulkanDevice& device);
	};
}