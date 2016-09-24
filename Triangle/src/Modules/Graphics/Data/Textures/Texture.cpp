#include "Modules/Graphics/Data/Textures/Texture.h"
#include <gli/gli.hpp>

namespace Graphics
{
	namespace Data
	{
		uint32_t Texture::getMemoryType(uint32_t typeBits, VkFlags properties, VkPhysicalDeviceMemoryProperties deviceMemoryProperties)
		{
			for (uint32_t i = 0; i < 32; i++)
			{
				if ((typeBits & 1) == 1)
				{
					if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
					{
						return i;
					}
				}
				typeBits >>= 1;
			}

			return 0;
		}

		void Texture::load(const char* filename, VkFormat format, VulkanPhysicalDevice& physicalDevice, VulkanDevice& device, VkCommandPool& pool, VulkanQueue& queue)
		{
			// Create command buffer for submitting image barriers
			// and converting tilings
			VkCommandBuffer cmdBuffer;
			VkCommandBufferAllocateInfo cmdBufInfo = {};
			cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmdBufInfo.commandPool = pool;
			cmdBufInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			cmdBufInfo.commandBufferCount = 1;

			VkResult err = vkAllocateCommandBuffers(device.vkDevice, &cmdBufInfo, &cmdBuffer);
			assert(!err);

			gli::texture2D tex2D(gli::load(filename));
			assert(!tex2D.empty());

			width = static_cast<uint32_t>(tex2D[0].dimensions().x);
			height = static_cast<uint32_t>(tex2D[0].dimensions().y);
			mipLevels = static_cast<uint32_t>(tex2D.levels());

			// Get device properites for the requested texture format
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(physicalDevice.vkPhysicalDevice, format, &formatProperties);

			// Only use linear tiling if requested (and supported by the device)
			// Support for linear tiling is mostly limited, so prefer to use
			// optimal tiling instead
			// On most implementations linear tiling will only support a very
			// limited amount of formats and features (mip maps, cubemaps, arrays, etc.)
			VkBool32 useStaging = !false;

			VkMemoryAllocateInfo memAllocInfo = {};
			memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAllocInfo.pNext = NULL;
			memAllocInfo.allocationSize = 0;
			memAllocInfo.memoryTypeIndex = 0;
			VkMemoryRequirements memReqs;

			// Use a separate command buffer for texture loading
			VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
			cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			cmdBufferBeginInfo.pNext = NULL;
			vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);

			if (useStaging)
			{
				// Create a host-visible staging buffer that contains the raw image data
				VkBuffer stagingBuffer;
				VkDeviceMemory stagingMemory;

				VkBufferCreateInfo bufCreateInfo = {};
				bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufCreateInfo.size = tex2D.size();
				// This buffer is used as a transfer source for the buffer copy
				bufCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				bufCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

				VkResult err = vkCreateBuffer(device.vkDevice, &bufCreateInfo, nullptr, &stagingBuffer);

				// Get memory requirements for the staging buffer (alignment, memory type bits)
				vkGetBufferMemoryRequirements(device.vkDevice, stagingBuffer, &memReqs);

				memAllocInfo.allocationSize = memReqs.size;
				// Get memory type index for a host visible buffer
				memAllocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, physicalDevice.memoryProperties());

				err = (vkAllocateMemory(device.vkDevice, &memAllocInfo, nullptr, &stagingMemory));
				assert(!err);
				err = (vkBindBufferMemory(device.vkDevice, stagingBuffer, stagingMemory, 0));
				assert(!err);

				// Copy texture data into staging buffer
				uint8_t *data;
				err = (vkMapMemory(device.vkDevice, stagingMemory, 0, memReqs.size, 0, (void **)&data));
				assert(!err);
				memcpy(data, tex2D.data(), tex2D.size());
				vkUnmapMemory(device.vkDevice, stagingMemory);

				// Setup buffer copy regions for each mip level
				std::vector<VkBufferImageCopy> bufferCopyRegions;
				uint32_t offset = 0;

				for (uint32_t i = 0; i < mipLevels; i++)
				{
					VkBufferImageCopy bufferCopyRegion = {};
					bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					bufferCopyRegion.imageSubresource.mipLevel = i;
					bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
					bufferCopyRegion.imageSubresource.layerCount = 1;
					bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(tex2D[i].dimensions().x);
					bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(tex2D[i].dimensions().y);
					bufferCopyRegion.imageExtent.depth = 1;
					bufferCopyRegion.bufferOffset = offset;

					bufferCopyRegions.push_back(bufferCopyRegion);

					offset += static_cast<uint32_t>(tex2D[i].size());
				}

				// Create optimal tiled target image
				VkImageCreateInfo imageCreateInfo = {};
				imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				imageCreateInfo.pNext = NULL;
				imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
				imageCreateInfo.format = format;
				imageCreateInfo.mipLevels = mipLevels;
				imageCreateInfo.arrayLayers = 1;
				imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
				imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
				imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
				imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
				imageCreateInfo.extent = { width, height, 1 };
				imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

				err = (vkCreateImage(device.vkDevice, &imageCreateInfo, nullptr, &image));
				assert(!err);

				vkGetImageMemoryRequirements(device.vkDevice, image, &memReqs);

				memAllocInfo.allocationSize = memReqs.size;

				memAllocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, physicalDevice.memoryProperties());
				err = (vkAllocateMemory(device.vkDevice, &memAllocInfo, nullptr, &deviceMemory));
				err = (vkBindImageMemory(device.vkDevice, image, deviceMemory, 0));

				VkImageSubresourceRange subresourceRange = {};
				subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				subresourceRange.baseMipLevel = 0;
				subresourceRange.levelCount = mipLevels;
				subresourceRange.layerCount = 1;

				// Image barrier for optimal image (target)
				// Optimal image will be used as destination for the copy
				// Create an image barrier object
				VkImageMemoryBarrier imageMemoryBarrier = {};
				imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageMemoryBarrier.pNext = NULL;
				// Some default values
				imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
				imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageMemoryBarrier.image = image;
				imageMemoryBarrier.subresourceRange = subresourceRange;
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				// Put barrier on top
				VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

				// Put barrier inside setup command buffer
				vkCmdPipelineBarrier(
					cmdBuffer,
					srcStageFlags,
					destStageFlags,
					0,
					0, nullptr,
					0, nullptr,
					1, &imageMemoryBarrier);


				// Copy mip levels from staging buffer
				vkCmdCopyBufferToImage(
					cmdBuffer,
					stagingBuffer,
					image,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					static_cast<uint32_t>(bufferCopyRegions.size()),
					bufferCopyRegions.data()
				);

				// Change texture image layout to shader read after all mip levels have been copied
				imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				// Create an image barrier object
				VkImageMemoryBarrier imageMemoryBarrier2 = {};
				imageMemoryBarrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageMemoryBarrier2.pNext = NULL;
				// Some default values
				imageMemoryBarrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageMemoryBarrier2.newLayout = imageLayout;
				imageMemoryBarrier2.image = image;
				imageMemoryBarrier2.subresourceRange = subresourceRange;
				imageMemoryBarrier2.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
				imageMemoryBarrier2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				// Put barrier on top
				VkPipelineStageFlags srcStageFlags2 = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				VkPipelineStageFlags destStageFlags2 = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

				// Put barrier inside setup command buffer
				vkCmdPipelineBarrier(
					cmdBuffer,
					srcStageFlags2,
					destStageFlags2,
					0,
					0, nullptr,
					0, nullptr,
					1, &imageMemoryBarrier2);
				// Submit command buffer containing copy and image layout commands
				err = (vkEndCommandBuffer(cmdBuffer));
				assert(!err);

				// Create a fence to make sure that the copies have finished before continuing
				VkFence copyFence;
				VkFenceCreateInfo fenceCreateInfo = {};
				fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceCreateInfo.flags = 0;
				err = (vkCreateFence(device.vkDevice, &fenceCreateInfo, nullptr, &copyFence));
				assert(!err);

				VkSubmitInfo submitInfo = {};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.pNext = NULL;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &cmdBuffer;

				err = (vkQueueSubmit(queue, 1, &submitInfo, copyFence));

				err = (vkWaitForFences(device.vkDevice, 1, &copyFence, VK_TRUE, 100000000000));

				vkDestroyFence(device.vkDevice, copyFence, nullptr);

				// Clean up staging resources
				vkFreeMemory(device.vkDevice, stagingMemory, nullptr);
				vkDestroyBuffer(device.vkDevice, stagingBuffer, nullptr);
			}
			else
			{
				// Prefer using optimal tiling, as linear tiling 
				// may support only a small set of features 
				// depending on implementation (e.g. no mip maps, only one layer, etc.)

				// Check if this support is supported for linear tiling
				assert(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

				VkImage mappableImage;
				VkDeviceMemory mappableMemory;

				VkImageCreateInfo imageCreateInfo = {};
				imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				imageCreateInfo.pNext = NULL;
				imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
				imageCreateInfo.format = format;
				imageCreateInfo.extent = { width, height, 1 };
				imageCreateInfo.mipLevels = 1;
				imageCreateInfo.arrayLayers = 1;
				imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
				imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
				imageCreateInfo.usage = (useStaging) ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : VK_IMAGE_USAGE_SAMPLED_BIT;
				imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

				// Load mip map level 0 to linear tiling image
				err = (vkCreateImage(device.vkDevice, &imageCreateInfo, nullptr, &mappableImage));
				assert(!err);

				// Get memory requirements for this image 
				// like size and alignment
				vkGetImageMemoryRequirements(device.vkDevice, mappableImage, &memReqs);
				// Set memory allocation size to required memory size
				memAllocInfo.allocationSize = memReqs.size;

				// Get memory type that can be mapped to host memory
				memAllocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, physicalDevice.memoryProperties());

				// Allocate host memory
				err = (vkAllocateMemory(device.vkDevice, &memAllocInfo, nullptr, &mappableMemory));
				assert(!err);

				// Bind allocated image for use
				err = (vkBindImageMemory(device.vkDevice, mappableImage, mappableMemory, 0));
				assert(!err);

				// Get sub resource layout
				// Mip map count, array layer, etc.
				VkImageSubresource subRes = {};
				subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				subRes.mipLevel = 0;

				VkSubresourceLayout subResLayout;
				void *data;

				// Get sub resources layout 
				// Includes row pitch, size offsets, etc.
				vkGetImageSubresourceLayout(device.vkDevice, mappableImage, &subRes, &subResLayout);

				// Map image memory
				err = (vkMapMemory(device.vkDevice, mappableMemory, 0, memReqs.size, 0, &data));
				assert(!err);

				// Copy image data into memory
				memcpy(data, tex2D[subRes.mipLevel].data(), tex2D[subRes.mipLevel].size());

				vkUnmapMemory(device.vkDevice, mappableMemory);

				// Linear tiled images don't need to be staged
				// and can be directly used as textures
				image = mappableImage;
				deviceMemory = mappableMemory;
				imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				// Setup image memory barrier
				VkImageSubresourceRange subresourceRange = {};
				subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				subresourceRange.baseMipLevel = 0;
				subresourceRange.levelCount = 1;
				subresourceRange.layerCount = 1;

				// Create an image barrier object
				VkImageMemoryBarrier imageMemoryBarrier = {};
				imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageMemoryBarrier.pNext = NULL;
				imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
				imageMemoryBarrier.newLayout = imageLayout;
				imageMemoryBarrier.image = image;
				imageMemoryBarrier.subresourceRange = subresourceRange;
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				// Put barrier on top
				VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

				// Put barrier inside setup command buffer
				vkCmdPipelineBarrier(
					cmdBuffer,
					srcStageFlags,
					destStageFlags,
					0,
					0, nullptr,
					0, nullptr,
					1, &imageMemoryBarrier);

				// Submit command buffer containing copy and image layout commands
				err = (vkEndCommandBuffer(cmdBuffer));
				assert(!err);

				VkFence nullFence = { VK_NULL_HANDLE };

				VkSubmitInfo submitInfo = {};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.pNext = NULL;
				submitInfo.waitSemaphoreCount = 0;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &cmdBuffer;

				err = (vkQueueSubmit(queue, 1, &submitInfo, nullFence));
				assert(!err);
				err = (vkQueueWaitIdle(queue));
				assert(!err);
			}

			// Create sampler
			VkSamplerCreateInfo sampler2 = {};
			sampler2.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			sampler2.magFilter = VK_FILTER_LINEAR;
			sampler2.minFilter = VK_FILTER_LINEAR;
			sampler2.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			sampler2.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampler2.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampler2.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampler2.mipLodBias = 0.0f;
			sampler2.compareOp = VK_COMPARE_OP_NEVER;
			sampler2.minLod = 0.0f;
			// Max level-of-detail should match mip level count
			sampler2.maxLod = (useStaging) ? (float)mipLevels : 0.0f;
			// Enable anisotropic filtering
			sampler2.maxAnisotropy = 8;
			sampler2.anisotropyEnable = VK_TRUE;
			sampler2.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			err = (vkCreateSampler(device.vkDevice, &sampler2, nullptr, &sampler));
			assert(!err);

			// Create image view
			// Textures are not directly accessed by the shaders and
			// are abstracted by image views containing additional
			// information and sub resource ranges
			VkImageViewCreateInfo view2 = {};
			view2.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view2.pNext = NULL;
			view2.image = VK_NULL_HANDLE;
			view2.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view2.format = format;
			view2.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
			view2.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
			// Linear tiling usually won't support mip maps
			// Only set mip map count if optimal tiling is used
			view2.subresourceRange.levelCount = (useStaging) ? mipLevels : 1;
			view2.image = image;
			err = (vkCreateImageView(device.vkDevice, &view2, nullptr, &view));
			assert(!err);
		}



	}
}
