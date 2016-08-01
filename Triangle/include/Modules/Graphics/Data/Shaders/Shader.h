#pragma once
#include "Modules\Graphics\VulkanWrappers\VulkanWrappers.h"
#include "Modules\Graphics\Data\Shaders\UniformMVP.h"
#include "Modules\Graphics\Data\Shaders\Uniform.h"

namespace Graphics
{
	namespace Data
	{
		class Shader
		{
		public:
			virtual void initialize() = 0;

			VkDescriptorSet           descriptorSet;
			VulkanDescriptorSetLayout descriptorSetLayout;
			VkDescriptorPool          descriptorPool;
		};

		class MeshShader : public Shader
		{
		public:
			virtual void initialize()
			{

			}

			Uniform uniformDataVS;
			UniformMVP uboVS;
		};
	}
}
