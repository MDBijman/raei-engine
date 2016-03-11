#pragma once

#include <vulkan\vulkan.h>
#include <VulkanPhysicalDevice.h>

class VulkanDevice
{
public:
	VulkanDevice(VulkanPhysicalDevice pd, uint32_t queueIndex)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueIndex;
		queueCreateInfo.queueCount = 1;
		float queuePriorities[1] = { 0.0f };
		queueCreateInfo.pQueuePriorities = &queuePriorities[0];

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		deviceCreateInfo.pEnabledFeatures = NULL;
		std::vector<const char*> enabledExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		deviceCreateInfo.enabledExtensionCount = (uint32_t)enabledExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();

		VkResult err = vkCreateDevice(pd, &deviceCreateInfo, nullptr, &device);
		assert(!err);
	}

	VkQueue queueAt(uint32_t index)
	{
		VkQueue queue;
		vkGetDeviceQueue(device, index, 0, &queue);
		return queue;
	}

	void bindBufferMemory(VkBuffer buffer, VkDeviceMemory memory)
	{
		VkResult error = vkBindBufferMemory(device, buffer, memory, 0);
		assert(!error);
	}

	VkQueue getQueueAtIndex(uint32_t index)
	{
		VkQueue queue;
		vkGetDeviceQueue(device, index, 0, &queue);
		return queue;
	}

	VkCommandPool createCommandPool(VkCommandPoolCreateInfo cmdPoolInfo)
	{
		VkCommandPool cmdPool;

		VkResult errorStatus = vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &cmdPool);
		assert(!errorStatus);

		return cmdPool;
	}

	std::vector<VkCommandBuffer> allocateCommandBuffers(VkCommandBufferAllocateInfo& info)
	{
		std::vector<VkCommandBuffer> buffers;
		buffers.resize(info.commandBufferCount);
		VkResult errorStatus = vkAllocateCommandBuffers(device, &info, buffers.data());
		assert(!errorStatus);

		return buffers;
	}

	void freeCommandBuffer(VkCommandPool& pool, std::vector<VkCommandBuffer> buffer)
	{
		vkFreeCommandBuffers(device, pool, buffer.size(), buffer.data());
	}


	VkMemoryRequirements getBufferMemoryRequirements(VkBuffer& buffer)
	{
		VkMemoryRequirements reqs;
		vkGetBufferMemoryRequirements(device, buffer, &reqs);
		return reqs;
	}

	VkDeviceMemory allocateMemory(VkMemoryAllocateInfo& info)
	{
		VkDeviceMemory mem;
		VkResult error = vkAllocateMemory(device, &info, nullptr, &mem);
		assert(!error);

		return mem;
	}

	void* mapMemory(VkDeviceMemory& memory, VkDeviceSize& size)
	{
		void* data;
		VkResult error = vkMapMemory(device, memory, 0, size, 0, &data);
		assert(!error);
		return data;
	}

	void unmapMemory(VkDeviceMemory& memory)
	{
		vkUnmapMemory(device, memory);
	}

	void bindImageMemory(VkImage& image, VkDeviceMemory memory)
	{
		VkResult error = vkBindImageMemory(device, image, memory, 0);
		assert(!error);
	}

	VkRenderPass createRenderPass(VkRenderPassCreateInfo& info)
	{
		VkRenderPass pass;
		VkResult error = vkCreateRenderPass(device, &info, nullptr, &pass);
		assert(!error);

		return pass;
	}
	
	VkImage createImage(VkImageCreateInfo& info)
	{
		VkImage image;
		VkResult error = vkCreateImage(device, &info, nullptr, &image);
		assert(!error);
		return image;
	}

	VkImageView createImageView(VkImageViewCreateInfo& info)
	{
		VkImageView view;
		VkResult error = vkCreateImageView(device, &info, nullptr, &view);
		return view;
	}

	VkMemoryRequirements getImageMemoryRequirements(VkImage& image)
	{
		VkMemoryRequirements reqs;
		vkGetImageMemoryRequirements(device, image, &reqs);
		return reqs;
	}

	VkPipelineCache createPipelineCache(VkPipelineCacheCreateInfo& info)
	{
		VkPipelineCache cache;
		VkResult error = vkCreatePipelineCache(device, &info, nullptr, &cache);
		assert(!error);
		return cache;
	}

	VkFramebuffer createFrameBuffer(VkFramebufferCreateInfo& info)
	{
		VkFramebuffer buffer;
		VkResult error = vkCreateFramebuffer(device, &info, nullptr, &buffer);
		assert(!error);
		return buffer;
	}

	VkBuffer createBuffer(VkBufferCreateInfo& info)
	{
		VkBuffer buffer;
		VkResult error = vkCreateBuffer(device, &info, nullptr, &buffer);
		assert(!error);

		return buffer;
	}

	VkDescriptorSetLayout createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo& info)
	{
		VkDescriptorSetLayout layout;
		VkResult error = vkCreateDescriptorSetLayout(device, &info, nullptr, &layout);
		assert(!error);
		return layout;
	}

	VkPipelineLayout createPipelineLayout(VkPipelineLayoutCreateInfo& info)
	{
		VkPipelineLayout layout;
		VkResult error = vkCreatePipelineLayout(device, &info, nullptr, &layout);
		assert(!error);

		return layout;
	}

	std::vector<VkPipeline> createGraphicsPipelines(VkPipelineCache& cache, VkGraphicsPipelineCreateInfo& info, uint32_t count)
	{
		std::vector<VkPipeline> pipelines;
		pipelines.resize(count);

		VkResult error = vkCreateGraphicsPipelines(device, cache, count, &info, nullptr, pipelines.data());
		assert(!error);
		
		return pipelines;
	}



	VkDescriptorPool createDescriptorPool (VkDescriptorPoolCreateInfo& info)
	{
		VkDescriptorPool pool;
		VkResult error = vkCreateDescriptorPool(device, &info, nullptr, &pool);
		assert(!error);
		return pool;
	}

	void allocateDescriptorSet(VkDescriptorSetAllocateInfo& info, VkDescriptorSet& set)
	{
		VkResult error = vkAllocateDescriptorSets(device, &info, &set);
		assert(!error);
	}

	void updateDescriptorSet(VkWriteDescriptorSet& set)
	{
		vkUpdateDescriptorSets(device, 1, &set, 0, NULL);
	}

	operator VkDevice() const { return device; };
private:
	VkDevice device;
};
