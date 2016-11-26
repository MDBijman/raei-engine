#pragma once
#include "Modules/ECS/ECS.h"
#include "Modules/Graphics/Graphics.h"
#include "Modules/Math/Matrices.h"
#include "Modules/Graphics/VulkanWrappers/VulkanContext.h"

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
		MeshShader(Graphics::VulkanContext& context, Graphics::Data::Texture& texture)
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

			createDescriptorSetLayout(context.device);

			prepareDescriptorPool(context.device);

			updateDescriptorSet(context.device, texture);
		}

		MeshShader(MeshShader&& other) noexcept : Component(other), Graphics::Data::Shader(other)
		{
			this->setLayoutBindings = other.setLayoutBindings;
			other.setLayoutBindings = nullptr;

			this->poolSizes = other.poolSizes;
			other.poolSizes = nullptr;

			this->uniformDataVS = other.uniformDataVS;
			this->uboVS = other.uboVS;
		}

		/**
		 * \brief Updates the uniform buffers for this shader.
		 * \param camera The camera to calculate the model view projection matrix from.
		 * \param device The VulkanDevice to map and unmap memory from.
		 */
		void updateUniformBuffers(Camera& camera, vk::Device& device)
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

		Graphics::Data::Uniform    uniformDataVS;
		Graphics::Data::UniformMVP uboVS;

	private:
		/**
		 * \brief Creates a descriptor set layout with a uniform buffer and an image sample.
		 * \param device The VulkanDevice to create the layout with.
		 */
		void createDescriptorSetLayout(vk::Device& device)
		{
			// Binding 0 : Uniform buffer (Vertex shader)
			setLayoutBindings = new std::vector<vk::DescriptorSetLayoutBinding>();
			setLayoutBindings->resize(2);
			setLayoutBindings->at(0) = vk::DescriptorSetLayoutBinding()
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setBinding(0)
				.setStageFlags(vk::ShaderStageFlagBits::eVertex);

			// Binding 1: Fragment shader image sampler
			setLayoutBindings->at(1) = vk::DescriptorSetLayoutBinding()
				.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
				.setDescriptorCount(1)
				.setBinding(1)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment);

			vk::DescriptorSetLayoutCreateInfo descriptorLayout;
			descriptorLayout
				.setBindingCount(2)
				.setPBindings(setLayoutBindings->data());

			descriptorSetLayout = device.createDescriptorSetLayout(descriptorLayout);
		}

		/**
		 * \brief Creates the descriptor pool needed to allocate descriptor sets.
		 * \param device The VulkanDevice to create the pool with.
		 */
		void prepareDescriptorPool(vk::Device& device)
		{
			// We need to tell the API the number of max. requested descriptors per type
			poolSizes = new std::vector<vk::DescriptorPoolSize>();
			poolSizes->push_back(vk::DescriptorPoolSize().setType(vk::DescriptorType::eUniformBuffer).setDescriptorCount(1));
			poolSizes->push_back(vk::DescriptorPoolSize().setType(vk::DescriptorType::eCombinedImageSampler).setDescriptorCount(1));

			// Create the global descriptor pool
			// All descriptors used in this example are allocated from this pool
			vk::DescriptorPoolCreateInfo descriptorPoolInfo;
			descriptorPoolInfo
				.setPoolSizeCount(2)
				.setPPoolSizes(poolSizes->data())
				// Set the max. number of sets that can be requested
				// Requesting descriptors beyond maxSets will result in an error
				.setMaxSets(1);

			descriptorPool = device.createDescriptorPool(descriptorPoolInfo);
		}

		/**
		 * \brief Updates the descriptor set to use the data from the given texture and this shader.
		 * \param device The VulkanDevice to allocate from.
		 * \param texture The texture to bind to the descriptor set.
		 */
		void updateDescriptorSet(vk::Device& device, Graphics::Data::Texture& texture)
		{
			// Update descriptor sets determining the shader binding points
			// For every binding point used in a shader there needs to be one
			// descriptor set matching that binding point

			vk::DescriptorSetAllocateInfo allocInfo;
			allocInfo
				.setDescriptorPool(descriptorPool)
				.setDescriptorSetCount(1)
				.setPSetLayouts(&descriptorSetLayout);
			descriptorSet = device.allocateDescriptorSets(allocInfo).at(0);

			std::vector<vk::WriteDescriptorSet> descriptorSets(2);

			// Binding 0: Uniform buffer

			descriptorSets[0] = vk::WriteDescriptorSet()
				.setDstSet(descriptorSet)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setPBufferInfo(&uniformDataVS.descriptor)
				.setDstBinding(0);

			vk::DescriptorImageInfo texDescriptor;
			texDescriptor.imageLayout = vk::ImageLayout::eGeneral;
			texDescriptor.sampler = texture.sampler;
			texDescriptor.imageView = texture.view;

			// Binding 1: Image sampler
			descriptorSets[1] = vk::WriteDescriptorSet()
				.setDstSet(descriptorSet)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
				.setPImageInfo(&texDescriptor)
				.setDstBinding(1);

			device.updateDescriptorSets(descriptorSets, nullptr);
		}

	private:
		std::vector<vk::DescriptorSetLayoutBinding>* setLayoutBindings;
		std::vector<vk::DescriptorPoolSize>* poolSizes;
	};
}
