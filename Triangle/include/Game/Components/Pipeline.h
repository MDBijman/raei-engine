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
		Pipeline(const std::string& location, vk::RenderPass& renderpass, vk::PipelineCache& cache, vk::Device& device, vk::PipelineVertexInputStateCreateInfo& vi, Graphics::Data::Texture& texture, MeshShader& shader)
		{
			// Create the pipeline layout that is used to generate the rendering pipelines that
			// are based on this descriptor set layout
			// In a more complex scenario you would have different pipeline layouts for different
			// descriptor set layouts that could be reused
			vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
			pipelineLayoutCreateInfo
				.setPSetLayouts(&shader.descriptorSetLayout)
				.setSetLayoutCount(1);
			vk::PipelineLayout layout = device.createPipelineLayout(pipelineLayoutCreateInfo);

			pipeline = Importers::Pipeline::load(location, layout, vi, renderpass, cache, device);

			// We need to tell the API the number of max. requested descriptors per type
			poolSizes.push_back(vk::DescriptorPoolSize().setType(vk::DescriptorType::eUniformBuffer).setDescriptorCount(1));
			poolSizes.push_back(vk::DescriptorPoolSize().setType(vk::DescriptorType::eCombinedImageSampler).setDescriptorCount(1));

			// Create the global descriptor pool
			// All descriptors used in this example are allocated from this pool
			vk::DescriptorPoolCreateInfo descriptorPoolInfo;
			descriptorPoolInfo
				.setPoolSizeCount(2)
				.setPPoolSizes(poolSizes.data())
				// Set the max. number of sets that can be requested
				// Requesting descriptors beyond maxSets will result in an error
				.setMaxSets(1);

			shader.descriptorPool = device.createDescriptorPool(descriptorPoolInfo);

			// Update descriptor sets determining the shader binding points
			// For every binding point used in a shader there needs to be one
			// descriptor set matching that binding point

			vk::DescriptorSetAllocateInfo allocInfo2;
			allocInfo2
				.setDescriptorPool(shader.descriptorPool)
				.setDescriptorSetCount(1)
				.setPSetLayouts(&shader.descriptorSetLayout);
			device.allocateDescriptorSets(&allocInfo2, &shader.descriptorSet);

			descriptorSets.resize(2);
			// Binding 0: Uniform buffer
			descriptorSets[0] = vk::WriteDescriptorSet()
				.setDstSet(shader.descriptorSet)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setPBufferInfo(&shader.uniformDataVS.descriptor)
				.setDstBinding(0);

			vk::DescriptorImageInfo texDescriptor;
			texDescriptor.imageLayout = vk::ImageLayout::eGeneral;
			texDescriptor.sampler = texture.sampler;
			texDescriptor.imageView = texture.view;

			// Binding 1: Image sampler
			descriptorSets[1] = vk::WriteDescriptorSet()
				.setDstSet(shader.descriptorSet)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
				.setPImageInfo(&texDescriptor)
				.setDstBinding(1);

			device.updateDescriptorSets(descriptorSets, nullptr);
		}

		Graphics::Pipeline pipeline;

		std::vector<vk::WriteDescriptorSet> descriptorSets;
		std::vector<vk::DescriptorPoolSize> poolSizes;
	};
}

