#pragma once

#include <assert.h>
#include <array>
#include <vulkan\vulkan.h>
#include "VulkanCommandBuffer.h"

class VulkanDevice
{
public:
	VulkanDevice(VulkanPhysicalDevice pd, uint32_t queueIndex)
	{
		std::array<float, 1> queuePriorities = { 0.0f };
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = queuePriorities.data();

		std::vector<const char*> enabledExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		deviceCreateInfo.pEnabledFeatures = NULL;
		deviceCreateInfo.enabledExtensionCount = (uint32_t)enabledExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();

		VkResult err = vkCreateDevice(pd, &deviceCreateInfo, nullptr, &vkDevice);
		assert(!err);
	}

	void waitIdle()
	{
		vkDeviceWaitIdle(vkDevice);
	}

	VkQueue queueAt(uint32_t index)
	{
		VkQueue queue;
		vkGetDeviceQueue(vkDevice, index, 0, &queue);
		return queue;
	}

	void bindBufferMemory(VkBuffer buffer, VkDeviceMemory memory)
	{
		VkResult error = vkBindBufferMemory(vkDevice, buffer, memory, 0);
		assert(!error);
	}

	VkQueue getQueueAtIndex(uint32_t index)
	{
		VkQueue queue;
		vkGetDeviceQueue(vkDevice, index, 0, &queue);
		return queue;
	}

	VkCommandPool createCommandPool(VkCommandPoolCreateInfo cmdPoolInfo)
	{
		VkCommandPool cmdPool;

		VkResult errorStatus = vkCreateCommandPool(vkDevice, &cmdPoolInfo, nullptr, &cmdPool);
		assert(!errorStatus);

		return cmdPool;
	}

	std::vector<VulkanCommandBuffer> allocateCommandBuffers(VkCommandBufferAllocateInfo& vkInfo)
	{
		std::vector<VkCommandBuffer> buffers;
		buffers.resize(vkInfo.commandBufferCount);
		VkResult errorStatus = vkAllocateCommandBuffers(vkDevice, &vkInfo, buffers.data());
		assert(!errorStatus);

		std::vector<VulkanCommandBuffer> wrappers;

		for (VkCommandBuffer b : buffers) 
			wrappers.push_back(VulkanCommandBuffer(b));

		return wrappers;
	}

	void freeCommandBuffers(VkCommandPool& pool, std::vector<VulkanCommandBuffer> buffer)
	{
		std::vector<VkCommandBuffer> raw_buffer;
		raw_buffer.resize(buffer.size());
		for(VulkanCommandBuffer b : buffer) 
			raw_buffer.push_back(b.vkBuffer);

		vkFreeCommandBuffers(vkDevice, pool, buffer.size(), raw_buffer.data());
	}

	void freeCommandBuffer(VkCommandPool& pool, VulkanCommandBuffer& buffer)
	{
		vkFreeCommandBuffers(vkDevice, pool, 1, &buffer.vkBuffer);
	}

	VkMemoryRequirements getBufferMemoryRequirements(VkBuffer& buffer)
	{
		VkMemoryRequirements reqs;
		vkGetBufferMemoryRequirements(vkDevice, buffer, &reqs);
		return reqs;
	}

	VkDeviceMemory allocateMemory(VkMemoryAllocateInfo& vkInfo)
	{
		VkDeviceMemory mem;
		VkResult error = vkAllocateMemory(vkDevice, &vkInfo, nullptr, &mem);
		assert(!error);
		return mem;
	}

	void* mapMemory(VkDeviceMemory& memory, int size)
	{
		void* data;
		VkResult error = vkMapMemory(vkDevice, memory, 0, size, 0, &data);
		assert(!error);
		return data;
	}

	void unmapMemory(VkDeviceMemory& memory)
	{
		vkUnmapMemory(vkDevice, memory);
	}

	void bindImageMemory(VkImage& image, VkDeviceMemory memory)
	{
		VkResult error = vkBindImageMemory(vkDevice, image, memory, 0);
		assert(!error);
	}

	VkRenderPass createRenderPass(VkRenderPassCreateInfo& vkInfo)
	{
		VkRenderPass pass;
		VkResult error = vkCreateRenderPass(vkDevice, &vkInfo, nullptr, &pass);
		assert(!error);

		return pass;
	}
	
	VkImage createImage(VkImageCreateInfo& vkInfo)
	{
		VkImage image;
		VkResult error = vkCreateImage(vkDevice, &vkInfo, nullptr, &image);
		assert(!error);
		return image;
	}

	VkImageView createImageView(VkImageViewCreateInfo& vkInfo)
	{
		VkImageView view;
		VkResult error = vkCreateImageView(vkDevice, &vkInfo, nullptr, &view);
		return view;
	}

	VkMemoryRequirements getImageMemoryRequirements(VkImage& image)
	{
		VkMemoryRequirements reqs;
		vkGetImageMemoryRequirements(vkDevice, image, &reqs);
		return reqs;
	}

	VkPipelineCache createPipelineCache(VkPipelineCacheCreateInfo& vkInfo)
	{
		VkPipelineCache cache;
		VkResult error = vkCreatePipelineCache(vkDevice, &vkInfo, nullptr, &cache);
		assert(!error);
		return cache;
	}

	VkFramebuffer createFrameBuffer(VkFramebufferCreateInfo& vkInfo)
	{
		VkFramebuffer buffer;
		VkResult error = vkCreateFramebuffer(vkDevice, &vkInfo, nullptr, &buffer);
		assert(!error);
		return buffer;
	}

	VkBuffer createBuffer(VkBufferCreateInfo& vkInfo)
	{
		VkBuffer buffer;
		VkResult error = vkCreateBuffer(vkDevice, &vkInfo, nullptr, &buffer);
		assert(!error);

		return buffer;
	}

	VkDescriptorSetLayout createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo& vkInfo)
	{
		VkDescriptorSetLayout layout;
		VkResult error = vkCreateDescriptorSetLayout(vkDevice, &vkInfo, nullptr, &layout);
		assert(!error);
		return layout;
	}

	VkPipelineLayout createPipelineLayout(VkPipelineLayoutCreateInfo& vkInfo)
	{
		VkPipelineLayout layout;
		VkResult error = vkCreatePipelineLayout(vkDevice, &vkInfo, nullptr, &layout);
		assert(!error);

		return layout;
	}

	std::vector<VkPipeline> createGraphicsPipelines(VkPipelineCache& cache, VkGraphicsPipelineCreateInfo& vkInfo, uint32_t count)
	{
		std::vector<VkPipeline> pipelines;
		pipelines.resize(count);

		VkResult error = vkCreateGraphicsPipelines(vkDevice, cache, count, &vkInfo, nullptr, pipelines.data());
		assert(!error);
		
		return pipelines;
	}

	VkDescriptorPool createDescriptorPool (VkDescriptorPoolCreateInfo& vkInfo)
	{
		VkDescriptorPool pool;
		VkResult error = vkCreateDescriptorPool(vkDevice, &vkInfo, nullptr, &pool);
		assert(!error);
		return pool;
	}

	void allocateDescriptorSet(VkDescriptorSetAllocateInfo& vkInfo, VkDescriptorSet& set)
	{
		VkResult error = vkAllocateDescriptorSets(vkDevice, &vkInfo, &set);
		assert(!error);
	}

	void updateDescriptorSet(VkWriteDescriptorSet& set)
	{
		vkUpdateDescriptorSets(vkDevice, 1, &set, 0, NULL);
	}

	VkSemaphore createSemaphore(VkSemaphoreCreateInfo& semaphoreCreateInfo)
	{
		VkSemaphore semaphore;
		VkResult error = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, nullptr, &semaphore);
		assert(!error);
		return semaphore;
	}

	VkDevice vkDevice;
};
