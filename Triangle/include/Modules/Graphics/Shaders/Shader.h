#pragma once
#include "Modules/Graphics/Shaders/Attributes/Attributes.h"
#include "Modules/Graphics/Shaders/Uniforms/Uniforms.h"
#include "Modules/Graphics/Shaders/Uniforms/Texture.h"

#include <vulkan/vulkan.hpp>

namespace Graphics
{

	namespace Data
	{
		template<class A, class U>
		class Shader;

		template<class... A, class... U>
		class Shader<Attributes<A...>, Uniforms<U...>>
		{
		public:
			Shader(Attributes<A...>&& attributes, Uniforms<U...>&& uniforms) : attributes(std::forward<Attributes<A...>>(attributes)), uniforms(std::forward<Uniforms<U...>>(uniforms))
			{}

			Shader(Shader&& other) : attributes(std::move(other.attributes)), uniforms(std::move(other.uniforms))
			{}

			Attributes<A...>& getAttributes()
			{
				return attributes;
			}

			Uniforms<U...>& getUniforms()
			{
				return uniforms;
			}

			void allocate(VulkanContext& context)
			{
				uniforms.allocate(context);
				attributes.allocate(context);
			}

			void draw(vk::CommandBuffer& cmdBuffer)
			{
				attributes.bind(cmdBuffer);
				if(attributes.isIndexed())
					cmdBuffer.drawIndexed(static_cast<uint32_t>(attributes.getIndices().value().data.size()), 1, 0, 0, 0);
				else
					cmdBuffer.draw(static_cast<uint32_t>(attributes.getVertices().data.size()), 1, 0, 0);
			}

		protected:
			Attributes<A...> attributes;
			Uniforms<U...> uniforms;
		};
	}
}
