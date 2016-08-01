#pragma once
#include <vulkan\vulkan.h>

class VulkanRectangle2D
{
public:
	VulkanRectangle2D()
	{
		vkRectangle = {};
	}

	VulkanRectangle2D& setWidth(uint32_t width)
	{
		vkRectangle.extent.width = width;
		return *this;
	}

	VulkanRectangle2D& setHeight(uint32_t height)
	{
		vkRectangle.extent.height = height;
		return *this;
	}

	VulkanRectangle2D& setX(int32_t x)
	{
		vkRectangle.offset.x = x;
		return *this;
	}

	VulkanRectangle2D& setY(int32_t y)
	{
		vkRectangle.offset.y = y;
		return *this;
	}

	VkRect2D vkRectangle;
};
