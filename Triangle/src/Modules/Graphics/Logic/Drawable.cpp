#include "Modules\Graphics\Logic\Drawable.h"

#include "Modules\Graphics\VulkanWrappers\VulkanWrappers.h"
#include "Modules\Importers\Obj.h"
#include "Modules\Importers\Pipeline.h"
#include "Modules\Importers\Shader.h"
#include "Modules\Math\Matrices.h"
#include "Modules\Importers\JSON.h"

#include <sstream>
#include <fstream>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\matrix_transform.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace Graphics
{
	Drawable::Drawable(Camera& camera, VulkanDevice& device, VulkanPhysicalDevice& physicalDevice, VkCommandPool& cmdPool, VulkanQueue& queue, VulkanRenderPass& renderpass, VkPipelineCache& cache, std::vector<VkFramebuffer>& framebuffers, VulkanSwapChain& swapchain)
	{
		addTexture("tree.dds", device, physicalDevice, cmdPool, queue);
		addObject("bad_car.obj", device, physicalDevice);
		prepareUniformBuffers(camera, device, physicalDevice);
		prepareDescriptorSetLayout(device);
		loadPipeline("./res/shaders/default-pipeline.json", renderpass, cache, device);
		prepareDescriptorPool(device);
		updateDescriptorSet(device);
		prepareCommandBuffers(camera, device, cmdPool, swapchain, renderpass, framebuffers);
	}

	void Drawable::addTexture(const std::string& name, VulkanDevice& device, VulkanPhysicalDevice& physicalDevice, VkCommandPool& cmdPool, VulkanQueue& queue)
	{
		texture.load(("./res/textures/" + name).c_str(), VK_FORMAT_BC3_UNORM_BLOCK, physicalDevice, device, cmdPool, queue);
	}

	void Drawable::addObject(const std::string& name, VulkanDevice& device, VulkanPhysicalDevice& physicalDevice)
	{
		Data::Mesh* m = Importers::Obj::load(("./res/models/" + name));

		mesh.vertices.setData(m->vertices.getData());
		mesh.vertices.upload(device, physicalDevice);

		mesh.indices.setData(m->indices.getData());
		mesh.indices.upload(device, physicalDevice);


		std::vector<VulkanVertexInputBindingDescription> bindingDescriptions(1);
		// Binding description
		bindingDescriptions[0]
			.setBinding(VERTEX_BUFFER_BIND_ID)
			.setStride(sizeof(Vertex))
			.setInputRate(VK_VERTEX_INPUT_RATE_VERTEX);

		mesh.vertices.vi.setVertexBindingDescriptions(bindingDescriptions);

		std::vector<VulkanVertexInputAttributeDescription> attributeDescriptions(3);
		// Attribute descriptions
		// Describes memory layout and shader attribute locations
		// Location 0 : Position
		attributeDescriptions[0]
			.setBinding(VERTEX_BUFFER_BIND_ID)
			.setOffset(0)
			.setLocation(0)
			.setFormat(VK_FORMAT_R32G32B32_SFLOAT);

		// Location 1 : UV
		attributeDescriptions[1]
			.setBinding(VERTEX_BUFFER_BIND_ID)
			.setLocation(1)
			.setFormat(VK_FORMAT_R32G32B32_SFLOAT)
			.setOffset(sizeof(float) * 3);

		// Location 2 : Normal
		attributeDescriptions[2]
			.setBinding(VERTEX_BUFFER_BIND_ID)
			.setLocation(2)
			.setFormat(VK_FORMAT_R32G32B32_SFLOAT)
			.setOffset(sizeof(float) * 5);

		// Assign to vertex buffer
		mesh.vertices.vi.setVertexAttributeDescriptions(attributeDescriptions);
	}

	void Drawable::prepareUniformBuffers(Camera& camera, VulkanDevice& device, VulkanPhysicalDevice& physicalDevice)
	{
		VkMemoryRequirements memReqs;

		// Vertex shader uniform buffer block
		VulkanBufferCreateInfo bufferInfo;
		bufferInfo
			.setSize(sizeof(shader.uboVS))
			.setUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		shader.uniformDataVS.buffer = device.createBuffer(bufferInfo);
		// Get memory requirements including size, alignment and memory type 
		memReqs = device.getBufferMemoryRequirements(shader.uniformDataVS.buffer);

		VulkanMemoryAllocateInfo allocInfo;
		allocInfo
			.setAllocationSize(memReqs.size)
			.setMemoryTypeIndex(physicalDevice.getMemoryPropertyIndex(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memReqs));

		shader.uniformDataVS.memory = device.allocateMemory(allocInfo.vkInfo);
		device.bindBufferMemory(shader.uniformDataVS.buffer, shader.uniformDataVS.memory);

		// Store information in the uniform's descriptor
		shader.uniformDataVS.descriptor.buffer = shader.uniformDataVS.buffer;
		shader.uniformDataVS.descriptor.offset = 0;
		shader.uniformDataVS.descriptor.range = sizeof(shader.uboVS);

		updateUniformBuffers(camera, device);
	}

	void Drawable::prepareDescriptorSetLayout(VulkanDevice& device)
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

		shader.descriptorSetLayout = device.createDescriptorSetLayout(descriptorLayout.vkInfo);

		// Create the pipeline layout that is used to generate the rendering pipelines that
		// are based on this descriptor set layout
		// In a more complex scenario you would have different pipeline layouts for different
		// descriptor set layouts that could be reused
		VulkanPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo
			.setSetLayouts(shader.descriptorSetLayout);


		solidPipeline.layout = device.createPipelineLayout(pipelineLayoutCreateInfo.vk);
	}

	void Drawable::loadPipeline(const std::string& location, VulkanRenderPass& renderpass, VkPipelineCache& cache, VulkanDevice& device)
	{
		solidPipeline = Importers::Pipeline::load(location, solidPipeline.layout, mesh.vertices.vi, renderpass, cache, device);
	}

	void Drawable::prepareDescriptorPool(VulkanDevice& device)
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

		shader.descriptorPool = device.createDescriptorPool(descriptorPoolInfo.vkInfo);
	}

	void Drawable::updateDescriptorSet(VulkanDevice& device)
	{
		// Update descriptor sets determining the shader binding points
		// For every binding point used in a shader there needs to be one
		// descriptor set matching that binding point

		VulkanDescriptorSetAllocateInfo allocInfo;
		allocInfo
			.setDescriptorPool(shader.descriptorPool)
			.setSetLayouts(shader.descriptorSetLayout);
		device.allocateDescriptorSet(allocInfo, shader.descriptorSet);

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

	void Drawable::prepareCommandBuffers(Camera& camera, VulkanDevice& device, VkCommandPool cmdPool, VulkanSwapChain& swapchain, VulkanRenderPass& renderPass, std::vector<VkFramebuffer>& framebuffers)
	{
		VulkanCommandBufferAllocateInfo info;
		info.setCommandPool(cmdPool)
			.setCommandBufferLevel(VK_COMMAND_BUFFER_LEVEL_PRIMARY)
			.setCommandBufferCount(swapchain.imageCount);

		commandBuffers = device.allocateCommandBuffers(info.vkInfo);

		VulkanCommandBufferBeginInfo cmdBufInfo;

		std::vector<VkClearValue> clearValues(2);
		clearValues.at(0) = VulkanClearValue()
			.setColor({ { 0.0f, 0.0f, 0.5f, 1.0f } }).vkValue;
		clearValues.at(1) = VulkanClearValue()
			.setDepthStencil({ 1.0f, 0 }).vkValue;

		// TODO create rectangle constructor to make this a single line
		VulkanRectangle2D renderArea;
		renderArea
			.setX(0)
			.setY(0)
			.setWidth(static_cast<uint32_t>(camera.getDimensions().x))
			.setHeight(static_cast<uint32_t>(camera.getDimensions().y));

		VulkanRenderPassBeginInfo renderPassBeginInfo;
		renderPassBeginInfo
			.setRenderPass(renderPass)
			.setRenderArea(renderArea.vkRectangle)
			.setClearValues(clearValues);

		for (int32_t i = 0; i < commandBuffers.size(); ++i)
		{
			// Set target frame buffer, std::vector<VkFramebuffer>& framebuffers
			renderPassBeginInfo.setFramebuffer(framebuffers.at(i));

			// Vulkan object preparation

			// Update dynamic viewport state
			VulkanViewport viewport;
			viewport
				.setWidth(camera.getDimensions().x)
				.setHeight(camera.getDimensions().y)
				.setMinDepth(0.0f)
				.setMaxDepth(1.0f);

			// Update dynamic scissor state
			VulkanRectangle2D scissor;
			scissor
				.setWidth(static_cast<uint32_t>(camera.getDimensions().x))
				.setHeight(static_cast<uint32_t>(camera.getDimensions().y))
				.setX(0)
				.setY(0);

			// Add a present memory barrier to the end of the command buffer
			// This will transform the frame buffer color attachment to a
			// new layout for presenting it to the windowing system integration 
			VulkanImageSubresourceRange subresourceRange;
			subresourceRange
				.setAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
				.setBaseMipLevel(0).setLevelCount(1).setBaseArrayLayer(0).setLayerCount(1);

			// Command chaining
			VulkanCommandBuffer& buffer = commandBuffers.at(i);
			buffer
				.beginCommandBuffer(cmdBufInfo.vkInfo)
				.beginRenderPass(renderPassBeginInfo.vkInfo, VK_SUBPASS_CONTENTS_INLINE)
				.setViewport(viewport.vkViewport)
				.setScissors(scissor.vkRectangle)
				// Bind descriptor sets describing shader binding points
				.bindDescriptorSet(solidPipeline.layout.vk, shader.descriptorSet)
				// Bind the pipeline with shaders, vertex and index buffers, and draw indexed triangles
				.bindPipeline(solidPipeline.vk)
				.bindVertexBuffers(mesh.vertices.getBuffer())
				.bindIndexBuffers(mesh.indices.getBuffer())
				.drawIndexed(mesh.indices.count)
				.endRenderPass()
				.end();
		}
	}

	void Drawable::updateUniformBuffers(Camera& camera, VulkanDevice& device)
	{
		// Update matrices
		shader.uboVS.projectionMatrix = Math::calculateProjection(camera.getFOV(), camera.getNearPlane(), camera.getFarPlane(), camera.getDimensions());
		shader.uboVS.viewMatrix = Math::calculateView(camera.getDirection(), camera.getUp(), camera.getPosition());
		shader.uboVS.modelMatrix = glm::mat4();
		shader.uboVS.modelMatrix = glm::scale(shader.uboVS.modelMatrix, glm::vec3(1, -1, 1));

		// Map uniform buffer and update it
		void *pData;
		pData = device.mapMemory(shader.uniformDataVS.memory, sizeof(shader.uboVS));
		memcpy(pData, &shader.uboVS, sizeof(shader.uboVS));
		device.unmapMemory(shader.uniformDataVS.memory);
	}

	std::vector<VulkanCommandBuffer>& Drawable::getCommandBuffers()
	{
		return this->commandBuffers;
	}

}
