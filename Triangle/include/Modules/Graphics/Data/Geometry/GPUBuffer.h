#pragma once

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
			virtual void upload(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice) = 0;
		};
	}
}
