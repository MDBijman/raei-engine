#pragma once

#include <vulkan\vulkan.h>
#include <assert.h>
#include <memory>
#include "VkSwapchain.h"

namespace VA
{
	class VulkanApplication
	{
	public:
		VkInstance								instance;

		VkPhysicalDevice						physicalDevice;
		VkPhysicalDeviceMemoryProperties		memoryProperties;
		std::vector<VkQueueFamilyProperties>	queueProperties;

		VkDevice								device; // TODO allow more

		std::unique_ptr<VulkanSwapChain>		swapchain;

		VulkanApplication(VkInstanceCreateInfo info)
		{
			VkResult err = vkCreateInstance(&info, nullptr, &instance);
			assert(!err);
		}

		// Initializes all physical devices of the instance
		void initPhysicalDevices()
		{
			uint32_t deviceCount;
			vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

			std::vector<VkPhysicalDevice> physicalDevices;
			physicalDevices.resize(deviceCount);

			vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

			// TODO allow for more than just the first physical device found
			physicalDevice = physicalDevices.at(0);
		}

		// Initializes the queueFamilyProperties of the physicalDevice 
		void initQueueFamilyProperties()
		{
			uint32_t queueCount;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);
			assert(queueCount >= 1);

			queueProperties.resize(queueCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProperties.data());
		}

		// Sets the instance, physical device, and device references of the swapchain
		void initSwapchain()
		{
			swapchain = std::make_unique<VulkanSwapChain>(instance, physicalDevice, device);
		}

		// Returns the index of the first queueFamily matching the given FlagBits
		uint32_t getQueueIndexOfType(VkQueueFlagBits bits)
		{
			uint32_t queueIndex;
			for (queueIndex = 0; queueIndex < queueProperties.size(); queueIndex++)
			{
				if (queueProperties.at(queueIndex).queueFlags & bits)
					break;
			}
			assert(queueIndex < queueProperties.size());
			return queueIndex;
		}

		// Returns the VkQueue at the given index
		VkQueue getQueueAtIndex(uint32_t index)
		{
			VkQueue queue;
			vkGetDeviceQueue(device, index, 0, &queue);
			return queue;
		}

		// Initializes the memory properties object using the physical device
		void initPhysicalDeviceMemoryProperties()
		{
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
		}

		// Initializes the device with the given info
		void initDevice(VkDeviceCreateInfo info)
		{
			VkResult err = vkCreateDevice(physicalDevice, &info, nullptr, &device);
			assert(!err);
		}

		// Queries the physical device for a specific format
		VkFormatProperties queryPhysicalDeviceFormatProperties(VkFormat format)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
			return props;
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

		void bindBufferMemory(VkBuffer buffer, VkDeviceMemory memory)
		{
			VkResult error = vkBindBufferMemory(device, buffer, memory, 0);
			assert(!error);
		}

		void freeCommandBuffer(VkCommandPool& pool, std::vector<VkCommandBuffer> buffer)
		{
			vkFreeCommandBuffers(device, pool, buffer.size(), buffer.data());
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


	private:
	};
}