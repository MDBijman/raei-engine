#pragma once
#include <vulkan/vulkan.h>

class VulkanSubpassDependency 
{
public:
	VulkanSubpassDependency() {}
	explicit VulkanSubpassDependency(VkSubpassDependency dep) : vk(dep) {}

	VkSubpassDependency vk;
};

