#pragma once
#include "VulkanVertexInputAttributeDescription.h"
#include "VulkanVertexInputBindingDescription.h"

#include <vulkan/vulkan.h>
#include <algorithm>

class VulkanPipelineVertexInputStateCreateInfo
{
public:
	VulkanPipelineVertexInputStateCreateInfo() 
	{
		vk.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vk.pNext = NULL;
		vk.flags = 0;
	}

	explicit VulkanPipelineVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo info) : vk(info) {}

	VulkanPipelineVertexInputStateCreateInfo& setVertexBindingDescriptions(std::vector<VulkanVertexInputBindingDescription> b)
	{
		bindings.clear();
		std::for_each(b.begin(), b.end(), [&](VulkanVertexInputBindingDescription a) {
			bindings.push_back(a.vk);
		});
		vk.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
		vk.pVertexBindingDescriptions = bindings.data();
		return *this;
	}

	VulkanPipelineVertexInputStateCreateInfo& setVertexAttributeDescriptions(std::vector<VulkanVertexInputAttributeDescription> a)
	{
		attributes.clear();
		std::for_each(a.begin(), a.end(), [&](VulkanVertexInputAttributeDescription a) {
			attributes.push_back(a.vk);
		});
		vk.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
		vk.pVertexAttributeDescriptions = attributes.data();
		return *this;
	}

	VkPipelineVertexInputStateCreateInfo vk;

private:
	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;
};