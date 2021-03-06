#pragma once
#include <vulkan/vulkan.hpp>

namespace graphics
{
	class Pipeline
	{
	public:
		Pipeline() : vk(nullptr) {}
		Pipeline(vk::Pipeline pipeline, vk::PipelineLayout pipelineLayout);



		vk::Pipeline vk;
		vk::PipelineLayout layout;

		std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachmentStates;
		std::vector<vk::DynamicState> dynamicStates;
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
		vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
		vk::PipelineViewportStateCreateInfo viewportInfo;
		vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
		vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
		vk::PipelineMultisampleStateCreateInfo multisampleInfo;
		std::vector<vk::PipelineShaderStageCreateInfo> shaderStagesInfo;
	};
}
