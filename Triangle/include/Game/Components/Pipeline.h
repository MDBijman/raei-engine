#pragma once
#include "Modules/ECS/ECS.h"
#include "Modules/Importers/PipelineImporter.h"
#include "Modules/Graphics/Graphics.h"
#include "Components.h"

namespace Components
{
	class Pipeline : public Component
	{
	public:
		Pipeline(const std::string& location, VulkanRenderPass& renderpass, VkPipelineCache& cache, VulkanDevice& device, VulkanPipelineVertexInputStateCreateInfo& vi, Graphics::Data::Texture& texture, Components::MeshShader& shader)
		{
	

			// Create the pipeline layout that is used to generate the rendering pipelines that
			// are based on this descriptor set layout
			// In a more complex scenario you would have different pipeline layouts for different
			// descriptor set layouts that could be reused
			VulkanPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
			pipelineLayoutCreateInfo
				.setSetLayouts(shader.descriptorSetLayout);


			VulkanPipelineLayout layout(device.createPipelineLayout(pipelineLayoutCreateInfo.vk));
			pipeline = Importers::Pipeline::load(location, layout, vi, renderpass, cache, device);

			// We need to tell the API the number of max. requested descriptors per type
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

			shader.descriptorPool = device.createDescriptorPool(descriptorPoolInfo.vkInfo);

			// Update descriptor sets determining the shader binding points
			// For every binding point used in a shader there needs to be one
			// descriptor set matching that binding point

			VulkanDescriptorSetAllocateInfo allocInfo2;
			allocInfo2
				.setDescriptorPool(shader.descriptorPool)
				.setSetLayouts(shader.descriptorSetLayout);
			device.allocateDescriptorSet(allocInfo2, shader.descriptorSet);

			std::vector<VkWriteDescriptorSet> descriptorSets(2);
			// Binding 0: Uniform buffer
			descriptorSets[0] = VulkanWriteDescriptorSet()
				.setDstSet(shader.descriptorSet)
				.setDescriptorCount(1)
				.setDescriptorType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				.setBufferInfo(shader.uniformDataVS.descriptor)
				.setDstBinding(0)
				.vkDescriptorSet;

			VkDescriptorImageInfo texDescriptor;
			texDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			texDescriptor.sampler = texture.sampler;
			texDescriptor.imageView = texture.view;

			// Binding 1: Image sampler
			descriptorSets[1] = VulkanWriteDescriptorSet()
				.setDstSet(shader.descriptorSet)
				.setDescriptorCount(1)
				.setDescriptorType(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
				.setImageInfo(texDescriptor)
				.setDstBinding(1)
				.vkDescriptorSet;

			device.updateDescriptorSet(descriptorSets);

		}

		Graphics::Pipeline pipeline;
	};
}

