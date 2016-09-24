#pragma once
#include <vulkan/vulkan.h>

namespace Graphics
{
	namespace Data
	{
		class Uniform
		{
		public:
			VkBuffer buffer;
			VkDeviceMemory memory;
			VkDescriptorBufferInfo descriptor;
		};
	}
}
