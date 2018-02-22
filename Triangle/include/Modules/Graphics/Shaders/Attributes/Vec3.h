#pragma once

namespace graphics
{
	namespace data
	{
		template<int LOCATION = 0, int OFFSET = 0>
		class Vec3
		{
		public:
			float x, y, z;

			static vk::VertexInputAttributeDescription getAttributeDescription()
			{
				return vk::VertexInputAttributeDescription()
					.setBinding(0)
					.setLocation(LOCATION)
					.setOffset(OFFSET)
					.setFormat(vk::Format::eR32G32B32Sfloat);
			}
		};
	}
}