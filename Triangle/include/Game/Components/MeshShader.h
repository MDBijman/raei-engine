#pragma once
#include "Modules/ECS/ECS.h"
#include "Modules/Graphics/Graphics.h"
#include "Modules/Math/Matrices.h"

namespace Components
{
	class MeshShader : public Component, public Graphics::Data::Shader
	{
	public:
		MeshShader(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice)
		{
			VkMemoryRequirements memReqs;

			// Vertex shader uniform buffer block
			VulkanBufferCreateInfo bufferInfo;
			bufferInfo
				.setSize(sizeof(uboVS))
				.setUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

			uniformDataVS.buffer = device.createBuffer(bufferInfo);
			// Get memory requirements including size, alignment and memory type 
			memReqs = device.getBufferMemoryRequirements(uniformDataVS.buffer);

			VulkanMemoryAllocateInfo allocInfo;
			allocInfo
				.setAllocationSize(memReqs.size)
				.setMemoryTypeIndex(physicalDevice.getMemoryPropertyIndex(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memReqs));

			uniformDataVS.memory = device.allocateMemory(allocInfo.vkInfo);
			device.bindBufferMemory(uniformDataVS.buffer, uniformDataVS.memory);

			// Store information in the uniform's descriptor
			uniformDataVS.descriptor.buffer = uniformDataVS.buffer;
			uniformDataVS.descriptor.offset = 0;
			uniformDataVS.descriptor.range = sizeof(uboVS);

			createDescriptorSetLayout(device);
		}

		void updateUniformBuffers(Camera& camera, VulkanDevice& device)
		{
			// Update matrices
			uboVS.projectionMatrix = Math::calculateProjection(camera.getFOV(), camera.getNearPlane(), camera.getFarPlane(), camera.getDimensions());
			uboVS.viewMatrix = Math::calculateView(camera.getDirection(), camera.getUp(), camera.getPosition());
			uboVS.modelMatrix = glm::mat4();
			uboVS.modelMatrix = glm::scale(uboVS.modelMatrix, glm::vec3(1, -1, 1));

			// Map uniform buffer and update it
			void *pData;
			pData = device.mapMemory(uniformDataVS.memory, sizeof(uboVS));
			memcpy(pData, &uboVS, sizeof(uboVS));
			device.unmapMemory(uniformDataVS.memory);
		}

		Graphics::Data::Uniform    uniformDataVS;
		Graphics::Data::UniformMVP uboVS;

	private:
		/**
		* Creates a descriptor set layout with a uniform buffer and an image sample.
		*
		* @param device The VulkanDevice to create the layout with.
		*/
		void createDescriptorSetLayout(VulkanDevice& device)
		{
			std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings(2);
			// Binding 0 : Uniform buffer (Vertex shader)
			setLayoutBindings[0] = VulkanDescriptorSetLayoutBinding()
				.setDescriptorType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				.setDescriptorCount(1)
				.setBinding(0)
				.setStageFlags(VK_SHADER_STAGE_VERTEX_BIT)
				.vkLayoutBinding;

			// Binding 1: Fragment shader image sampler
			setLayoutBindings[1] = VulkanDescriptorSetLayoutBinding()
				.setDescriptorType(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
				.setDescriptorCount(1)
				.setBinding(1)
				.setStageFlags(VK_SHADER_STAGE_FRAGMENT_BIT)
				.vkLayoutBinding;

			VulkanDescriptorSetLayoutCreateInfo descriptorLayout;
			descriptorLayout
				.setBindings(setLayoutBindings);

			descriptorSetLayout = device.createDescriptorSetLayout(descriptorLayout.vkInfo);
		}

		/**
		*
		*/
		void initializeUniformDataVS()
		{

		}
	};
}
