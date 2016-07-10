#pragma once
#include <vulkan\vulkan.h>

class VulkanDescriptorSetLayout
{
public:
	VulkanDescriptorSetLayout() {}
	VulkanDescriptorSetLayout(VkDescriptorSetLayout vkl) : vk(vkl) {}

	VkDescriptorSetLayout vk;
};
