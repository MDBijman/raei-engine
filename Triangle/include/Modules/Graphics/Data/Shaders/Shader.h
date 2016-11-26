#pragma once
#include <vulkan/vulkan.hpp>

namespace Graphics
{
	namespace Data
	{
		class Shader
		{
		public:
			vk::DescriptorSet           descriptorSet;
			vk::DescriptorSetLayout descriptorSetLayout;
			vk::DescriptorPool          descriptorPool;
		};
	}
}
