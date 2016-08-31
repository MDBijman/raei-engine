#pragma once
#include "VulkanPipelineVertexInputStateCreateInfo.h"
#include "VulkanVertexInputAttributeDescription.h"
#include "VulkanVertexInputBindingDescription.h"

#include <vulkan\vulkan.h>
#include <algorithm>
#include <iterator>

class VulkanPipelineVertexInputStateCreateInfo
{
public:
	VulkanPipelineVertexInputStateCreateInfo() 
	{
		vk.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vk.pNext = NULL;
		vk.flags = 0;
	}

	VulkanPipelineVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo info) : vk(info) {}

	VulkanPipelineVertexInputStateCreateInfo& setVertexBindingDescriptions(std::vector<VulkanVertexInputBindingDescription> b)
	{
		bindings.clear();
		std::transform(b.begin(), b.end(), std::back_inserter(bindings), [](VulkanVertexInputBindingDescription& a) {
			return a.vk;
		});
		vk.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
		vk.pVertexBindingDescriptions = bindings.data();
		return *this;
	}

	VulkanPipelineVertexInputStateCreateInfo& setVertexAttributeDescriptions(std::vector<VulkanVertexInputAttributeDescription> a)
	{
		attributes.clear();
		std::transform(a.begin(), a.end(), std::back_inserter(attributes), [](VulkanVertexInputAttributeDescription& a) {
			return a.vk;
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