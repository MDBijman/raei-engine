#pragma once
#include <vulkan/vulkan.hpp>

namespace graphics
{
	namespace data
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

			const vk::Buffer& getBuffer() const 
			{
				return buf;
			}

		protected:
			vk::Buffer buf;
			vk::DeviceMemory mem;
		};
	}
}
