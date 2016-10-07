#pragma once
#include <vulkan/vulkan.h>

class VulkanDescriptorSetLayout
{
public:
	VulkanDescriptorSetLayout(): vk(VK_NULL_HANDLE) {}

	explicit VulkanDescriptorSetLayout(VkDescriptorSetLayout vkl) : vk(vkl) {}

	VkDescriptorSetLayout vk;
};
