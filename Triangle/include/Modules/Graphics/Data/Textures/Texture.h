#pragma once
#include <vulkan/vulkan.hpp>

namespace Graphics
{
	namespace Data
	{
		class Texture
		{
		public:
			vk::Sampler sampler;
			vk::Image image;
			vk::ImageLayout imageLayout;
			vk::DeviceMemory deviceMemory;
			vk::ImageView view;
			uint32_t width, height;
			uint32_t mipLevels;

			// Get appropriate memory type index for a memory allocation
			static uint32_t getMemoryType(uint32_t typeBits, VkFlags properties, VkPhysicalDeviceMemoryProperties deviceMemoryProperties);

			void load(const char* filename, vk::Format format, vk::PhysicalDevice& physicalDevice, vk::Device& device, vk::CommandPool& pool, vk::Queue& queue);
		};
	}
}
