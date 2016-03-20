#pragma once
#include <vulkan\vulkan.h>

class VulkanClearValue
{
public:
	VulkanClearValue() {}
	VulkanClearValue(VkClearValue vulkanValue) : vkValue(vulkanValue) {}

	VulkanClearValue& setColor(VkClearColorValue color)
	{
		vkValue.color = color;
		return *this;
	}

	VulkanClearValue& setDepthStencil(VkClearDepthStencilValue stencil)
	{
		vkValue.depthStencil = stencil;
		return *this;
	}

	VkClearValue vkValue;

	static std::vector<VkClearValue> toVulkanVector(std::vector<VulkanClearValue> vector)
	{
		std::vector<VkClearValue> vulkanVector(vector.size());
		std::transform(vector.begin(), vector.end(), vulkanVector.begin(), [](VulkanClearValue value) { return value.vkValue; });
		return vulkanVector;
	}

	static std::vector<VulkanClearValue> toWrapperVector(std::vector<VkClearValue> vector)
	{
		std::vector<VulkanClearValue> wrapperVector(vector.size());
		std::transform(vector.begin(), vector.end(), wrapperVector.begin(), [](VkClearValue value) { return VulkanClearValue(value); });
		return wrapperVector;
	}
};