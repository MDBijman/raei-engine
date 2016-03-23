#pragma once
#include <vulkan\vulkan.h>
#include <string>

class VulkanPipelineShaderStageCreateInfo
{
public:
	VulkanPipelineShaderStageCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanPipelineShaderStageCreateInfo& setStage(VkShaderStageFlagBits bits)
	{
		vkInfo.stage = bits;
		return *this;
	}

	VulkanPipelineShaderStageCreateInfo& setModule(VkShaderModule module)
	{
		vkInfo.module = module;
		return *this;
	}

	VulkanPipelineShaderStageCreateInfo& setName(std::string& name)
	{
		vkInfo.pName = name.c_str();
		return *this;
	}

	VkPipelineShaderStageCreateInfo vkInfo;
};
