#pragma once
#include <vulkan/vulkan.h>

class VulkanDescriptorSetLayout
{
public:
	VulkanDescriptorSetLayout(): vk(nullptr) {}

	explicit VulkanDescriptorSetLayout(VkDescriptorSetLayout vkl) : vk(vkl) {}

	VkDescriptorSetLayout vk;
};
