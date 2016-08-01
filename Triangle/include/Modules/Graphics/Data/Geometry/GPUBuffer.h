#pragma once

namespace Graphics
{
	namespace Data
	{
		class GPUBuffer
		{
		public:
			/*
				Uploads the data of this buffer to the GPU.
			*/
			virtual void upload(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice) = 0;
		};
	}
}
