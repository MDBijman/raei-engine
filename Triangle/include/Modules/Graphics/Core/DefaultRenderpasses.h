#pragma once
#include <vulkan/vulkan.hpp>
#include "VulkanContext.h"

namespace speck::graphics
{
	vk::RenderPass get_clear_pass(::graphics::VulkanContext& context, vk::Format& depthFormat, vk::Format& colorFormat);
	vk::RenderPass get_draw_pass(::graphics::VulkanContext& context, vk::Format& depthFormat, vk::Format& colorFormat);
}