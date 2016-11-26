#pragma once
#include <vulkan/vulkan.hpp>

namespace Graphics
{
	namespace Data
	{
		class GPUBuffer
		{
		public:
			virtual ~GPUBuffer()
			{
			}

			/*
				Uploads the data of this buffer to the GPU.
			*/
			virtual void upload(vk::Device& device, vk::PhysicalDevice& physicalDevice) = 0;
		};
	}
}
