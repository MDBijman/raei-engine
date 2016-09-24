#pragma once
#include "Modules/Graphics/VulkanWrappers/VulkanWrappers.h"
#include "Modules/Graphics/Data/Shaders/Uniform.h"

namespace Graphics
{
	namespace Data
	{
		class Shader
		{
		public:
			VkDescriptorSet           descriptorSet;
			VulkanDescriptorSetLayout descriptorSetLayout;
			VkDescriptorPool          descriptorPool;
		};
	}
}
