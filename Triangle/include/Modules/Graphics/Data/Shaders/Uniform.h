#pragma once

namespace Graphics
{
	namespace Data
	{
		class Uniform
		{
		public:
			vk::Buffer buffer;
			vk::DeviceMemory memory;
			vk::DescriptorBufferInfo descriptor;
		};
	}
}
