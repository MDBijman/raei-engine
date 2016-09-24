#pragma once
#include <vulkan/vulkan.h>

class VulkanPipelineShaderStageCreateInfo
{
public:
	VulkanPipelineShaderStageCreateInfo()
	{
		vk = {};
		vk.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vk.pNext = NULL;
	}

	VulkanPipelineShaderStageCreateInfo& setStage(VkShaderStageFlagBits bits)
	{
		vk.stage = bits;
		return *this;
	}

	VulkanPipelineShaderStageCreateInfo& setModule(VkShaderModule module)
	{
		vk.module = module;
		return *this;
	}

	VulkanPipelineShaderStageCreateInfo& setName(std::string name)
	{
		this->name = name;
		vk.pName = this->name.c_str();
		return *this;
	}

	VkPipelineShaderStageCreateInfo vk;

private:
	std::string name;
};
