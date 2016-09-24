#pragma once
#include "Modules/ECS/ECS.h"
#include "Modules/Graphics/Graphics.h"
#include "Modules/Math/Matrices.h"

namespace Components
{
	/**
	 * \brief MeshShader provides an implementation for the mesh fragment and vertex shaders.
	 */
	class MeshShader : public Component, public Graphics::Data::Shader
	{
	public:
		/**
		 * \brief Creates a new MeshShader with the required uniform data objects, descriptor set, descriptor set layout, and descriptor pool.
		 * \param device The VulkanDevice to make vulkan objects with.
		 * \param physicalDevice The VulkanPhysicalDevice to make vulkan objects.
		 */
		MeshShader(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice, Graphics::Data::Texture& texture)
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

			prepareDescriptorPool(device);

			updateDescriptorSet(device, texture);
		}

		/**
		 * \brief Updates the uniform buffers for this shader.
		 * \param camera The camera to calculate the model view projection matrix from.
		 * \param device The VulkanDevice to map and unmap memory from.
		 */
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
		 * \brief Creates a descriptor set layout with a uniform buffer and an image sample.
		 * \param device The VulkanDevice to create the layout with.
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
		 * \brief Creates the descriptor pool needed to allocate descriptor sets.
		 * \param device The VulkanDevice to create the pool with.
		 */
		void prepareDescriptorPool(VulkanDevice& device)
		{
			// We need to tell the API the number of max. requested descriptors per type
			VulkanDescriptorPoolSize typeCounts;

			std::vector<VkDescriptorPoolSize> poolSize
			{
				VulkanDescriptorPoolSize().setType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER).setDescriptorCount(1).vkPoolSize,
				VulkanDescriptorPoolSize().setType(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER).setDescriptorCount(1).vkPoolSize
			};

			// Create the global descriptor pool
			// All descriptors used in this example are allocated from this pool
			VulkanDescriptorPoolCreateInfo descriptorPoolInfo;
			descriptorPoolInfo
				.setPoolSizes(poolSize)
				// Set the max. number of sets that can be requested
				// Requesting descriptors beyond maxSets will result in an error
				.setMaxSets(1);

			descriptorPool = device.createDescriptorPool(descriptorPoolInfo.vkInfo);
		}

		/**
		 * \brief Updates the descriptor set to use the data from the given texture and this shader.
		 * \param device The VulkanDevice to allocate from.
		 * \param texture The texture to bind to the descriptor set.
		 */
		void updateDescriptorSet(VulkanDevice& device, Graphics::Data::Texture& texture)
		{
			// Update descriptor sets determining the shader binding points
			// For every binding point used in a shader there needs to be one
			// descriptor set matching that binding point

			VulkanDescriptorSetAllocateInfo allocInfo;
			allocInfo
				.setDescriptorPool(descriptorPool)
				.setSetLayouts(descriptorSetLayout);
			device.allocateDescriptorSet(allocInfo, descriptorSet);

			std::vector<VkWriteDescriptorSet> descriptorSets(2);

			// Binding 0: Uniform buffer

			descriptorSets[0] = VulkanWriteDescriptorSet()
				.setDstSet(descriptorSet)
				.setDescriptorCount(1)
				.setDescriptorType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				.setBufferInfo(uniformDataVS.descriptor)
				.setDstBinding(0)
				.vkDescriptorSet;

			VkDescriptorImageInfo texDescriptor;
			texDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			texDescriptor.sampler = texture.sampler;
			texDescriptor.imageView = texture.view;

			// Binding 1: Image sampler
			descriptorSets[1] = VulkanWriteDescriptorSet()
				.setDstSet(descriptorSet)
				.setDescriptorCount(1)
				.setDescriptorType(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
				.setImageInfo(texDescriptor)
				.setDstBinding(1)
				.vkDescriptorSet;

			device.updateDescriptorSet(descriptorSets);
		}
	};
}
