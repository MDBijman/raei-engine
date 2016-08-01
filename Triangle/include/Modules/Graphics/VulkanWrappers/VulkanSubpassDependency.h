#pragma once
#include <vulkan\vulkan.h>
#include <string>

class VulkanSubpassDependency 
{
public:
	VulkanSubpassDependency() {}
	VulkanSubpassDependency(VkSubpassDependency dep) : vk(dep) {}

	VkSubpassDependency vk;
};

