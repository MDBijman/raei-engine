#pragma once
#include <vector>

#include "VulkanContext.h"
#include "Renderer.h"
#include "Drawable.h"

namespace graphics
{
	class Frame
	{
		friend class Renderer;

	public:
		template<class Shader>
		Frame& add_drawable(speck::graphics::drawable<Shader>& drawable)
		{
			buffers.push_back(&drawable.buffers().at(buffer_index));
			return *this;
		}

	private:
		Frame(uint32_t bi) : buffer_index(bi) {}

		uint32_t buffer_index;
		std::vector<vk::CommandBuffer*> buffers;
	};
}