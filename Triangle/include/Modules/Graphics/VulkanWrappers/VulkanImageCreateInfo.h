#pragma once
#include <vulkan/vulkan.h>

class VulkanImageCreateInfo
{
public:
	VulkanImageCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanImageCreateInfo& setImageType(VkImageType type)
	{
		vkInfo.imageType = type;
		return *this;
	}

	VulkanImageCreateInfo& setFormat(VkFormat format)
	{
		vkInfo.format = format;
		return *this;
	}

	VulkanImageCreateInfo& setExtent(VkExtent3D extent)
	{
		vkInfo.extent = extent;
		return *this;
	}
	
	VulkanImageCreateInfo& setMipLevels(uint32_t levels)
	{
		vkInfo.mipLevels = levels;
		return *this;
	}
	
	VulkanImageCreateInfo& setArrayLayers(uint32_t layers)
	{
		vkInfo.arrayLayers = layers;
		return *this;
	}

	VulkanImageCreateInfo& setSamples(VkSampleCountFlagBits samples)
	{
		vkInfo.samples = samples;
		return *this;
	}

	VulkanImageCreateInfo& setTiling(VkImageTiling tiling)
	{
		vkInfo.tiling = tiling;
		return *this;
	}


	VulkanImageCreateInfo& setUsage(VkImageUsageFlagBits usage)
	{
		vkInfo.usage = usage;
		return *this;
	}

	VulkanImageCreateInfo& setUsage(int bits)
	{
		vkInfo.usage = bits;
		return *this;
	}

	VulkanImageCreateInfo& setFlags(VkImageCreateFlags flags)
	{
		vkInfo.flags = flags;
		return *this;
	}




	VkImageCreateInfo vkInfo;
};