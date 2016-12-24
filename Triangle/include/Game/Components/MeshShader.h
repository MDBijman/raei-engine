#pragma once
#include "Modules/ECS/ECS.h"
#include "Modules/Graphics/Graphics.h"
#include "Modules/Math/Matrices.h"
#include "Modules/Graphics/VulkanWrappers/VulkanContext.h"

namespace Components
{
	using MeshAttributeType = Graphics::Data::Attributes<
		Graphics::Data::Vec3<0, 0>, Graphics::Data::Vec2<1, 3>, Graphics::Data::Vec3<2, 5>
	>;

	using MeshUniformType = Graphics::Data::Uniforms<
		Graphics::Data::UniformBuffer<0, vk::ShaderStageFlagBits::eVertex>,
		Graphics::Data::Texture<1, vk::ShaderStageFlagBits::eFragment>
	>;

	using MeshShaderType = Graphics::Data::Shader<MeshAttributeType, MeshUniformType>;


	/**
	 * \brief MeshShader provides an implementation for the mesh fragment and vertex shaders.
	 */
	class MeshShader : public Component, public MeshShaderType
	{
	public:
		/**
		 * \brief Creates a new MeshShader with the required uniform data objects, descriptor set, descriptor set layout, and descriptor pool.
		 * \param device The VulkanDevice to make vulkan objects with.
		 * \param physicalDevice The VulkanPhysicalDevice to make vulkan objects.
		 */
		MeshShader(Graphics::VulkanContext& context, MeshAttributeType attributes, MeshUniformType uniforms) : MeshShaderType(std::move(attributes), std::move(uniforms))
		{
			allocateUniform(context);
		}

		MeshShader(MeshShader&& other) noexcept : Component(std::move(other)), MeshShaderType(std::move(other))
		{

		}

		/**
		* \brief Updates the uniform buffers for this shader.
		* \param camera The camera to calculate the model view projection matrix from.
		* \param device The VulkanDevice to map and unmap memory from.
		*/
		void updateUniformBuffers(Camera& camera, vk::Device& device) override
		{
			// Update matrices
			uboVS.projectionMatrix = Math::calculateProjection(camera.getFOV(), camera.getNearPlane(), camera.getFarPlane(), camera.getDimensions());
			uboVS.viewMatrix = Math::calculateView(camera.getDirection(), camera.getUp(), camera.getPosition());
			uboVS.modelMatrix = glm::mat4();
			uboVS.modelMatrix = glm::scale(uboVS.modelMatrix, glm::vec3(1, -1, 1));

			// Map uniform buffer and update it
			void *pData;
			pData = device.mapMemory(uniformDataVS.memory, 0, sizeof uboVS);
			memcpy(pData, &uboVS, sizeof uboVS);
			device.unmapMemory(uniformDataVS.memory);
		}

	private:
		void allocateUniform(Graphics::VulkanContext& context)
		{
			// Vertex shader uniform buffer block
			vk::BufferCreateInfo bufferInfo;
			bufferInfo
				.setSize(sizeof(uboVS))
				.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);

			uniformDataVS.buffer = context.device.createBuffer(bufferInfo);

			auto allocInfo = context.getMemoryRequirements(uniformDataVS.buffer, vk::MemoryPropertyFlagBits::eHostVisible);

			uniformDataVS.memory = context.device.allocateMemory(allocInfo);
			context.device.bindBufferMemory(uniformDataVS.buffer, uniformDataVS.memory, 0);

			// Store information in the uniform's descriptor
			uniformDataVS.descriptor.buffer = uniformDataVS.buffer;
			uniformDataVS.descriptor.offset = 0;
			uniformDataVS.descriptor.range = sizeof(uboVS);
		}

		Graphics::Data::Uniform		uniformDataVS;
		Graphics::Data::UniformMVP	uboVS;
	};
}
