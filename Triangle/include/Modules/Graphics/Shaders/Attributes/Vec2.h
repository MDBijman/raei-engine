#pragma once

namespace Graphics
{
	namespace Data
	{
		template<int LOCATION = 0, int OFFSET = 0>
		class Vec2
		{
		public:
			float x, y;
		
			static vk::VertexInputAttributeDescription getAttributeDescription()
			{
				return vk::VertexInputAttributeDescription()
					.setBinding(0)
					.setLocation(LOCATION)
					.setOffset(OFFSET)
					.setFormat(vk::Format::eR32G32Sfloat);
			}
		}; 
	}
}