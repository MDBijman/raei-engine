#pragma once
#include <vulkan\VULKAN.HPP>
#include <gli/gli.hpp>

namespace Graphics
{
	namespace Data
	{
		template<int BINDING, vk::ShaderStageFlagBits STAGE>
		class Texture
		{
		public:
			Texture()
			{
				textureDescriptor = new vk::DescriptorImageInfo();
				textureDescriptor->setImageLayout(vk::ImageLayout::eGeneral)
					.setSampler(sampler)
					.setImageView(view);
			}

			Texture(Texture&& other)
			{
				this->textureDescriptor = other.textureDescriptor;
				other.textureDescriptor = nullptr;
				sampler = other.sampler;
				image = other.image;
				imageLayout = other.imageLayout;
				deviceMemory = other.deviceMemory;
				view = other.view;
				width = other.width;
				height = other.height;
				mipLevels = other.mipLevels;
			}

			void upload(vk::Device& device, vk::PhysicalDevice& physicalDevice)
			{}

			vk::WriteDescriptorSet getWriteDescriptorSet()
			{
				return vk::WriteDescriptorSet()
					.setDescriptorCount(1)
					.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
					.setPImageInfo(textureDescriptor)
					.setDstBinding(BINDING);
			}

			static vk::DescriptorSetLayoutBinding getDescriptorSetLayoutBinding()
			{
				return vk::DescriptorSetLayoutBinding()
					.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
					.setDescriptorCount(1)
					.setBinding(BINDING)
					.setStageFlags(STAGE);
			}

			static vk::DescriptorPoolSize getDescriptorPoolSize()
			{
				return vk::DescriptorPoolSize()
					.setType(vk::DescriptorType::eCombinedImageSampler)
					.setDescriptorCount(1);
			}

			vk::Sampler sampler;
			vk::Image image;
			vk::ImageLayout imageLayout;
			vk::DeviceMemory deviceMemory;
			vk::ImageView view;
			uint32_t width, height;
			uint32_t mipLevels;
			vk::DescriptorImageInfo* textureDescriptor;

			// Get appropriate memory type index for a memory allocation
			uint32_t getMemoryType(uint32_t typeBits, VkFlags properties, VkPhysicalDeviceMemoryProperties deviceMemoryProperties)
			{
				for(uint32_t i = 0; i < 32; i++)
				{
					if((typeBits & 1) == 1)
					{
						if((deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
						{
							return i;
						}
					}
					typeBits >>= 1;
				}

				return 0;
			}

			void load(const char* filename, vk::Format format, vk::PhysicalDevice& physicalDevice, vk::Device& device, vk::CommandPool& pool, vk::Queue& queue)
			{
				// Create command buffer for submitting image barriers
				// and converting tilings
				vk::CommandBuffer cmdBuffer;
				vk::CommandBufferAllocateInfo cmdBufInfo = {};
				cmdBufInfo.commandPool = pool;
				cmdBufInfo.level = vk::CommandBufferLevel::ePrimary;
				cmdBufInfo.commandBufferCount = 1;

				cmdBuffer = device.allocateCommandBuffers(cmdBufInfo).at(0);

				gli::texture2D tex2D(gli::load(filename));
				assert(!tex2D.empty());

				width = static_cast<uint32_t>(tex2D[0].dimensions().x);
				height = static_cast<uint32_t>(tex2D[0].dimensions().y);
				mipLevels = static_cast<uint32_t>(tex2D.levels());

				// Get device properites for the requested texture format
				VkFormatProperties formatProperties;
				formatProperties = physicalDevice.getFormatProperties(format);

				// Only use linear tiling if requested (and supported by the device)
				// Support for linear tiling is mostly limited, so prefer to use
				// optimal tiling instead
				// On most implementations linear tiling will only support a very
				// limited amount of formats and features (mip maps, cubemaps, arrays, etc.)
				vk::Bool32 useStaging = !false;

				vk::MemoryAllocateInfo memAllocInfo = {};
				vk::MemoryRequirements memReqs;

				// Use a separate command buffer for texture loading
				vk::CommandBufferBeginInfo cmdBufferBeginInfo = {};
				cmdBuffer.begin(cmdBufferBeginInfo);

				if(useStaging)
				{
					// Create a host-visible staging buffer that contains the raw image data
					vk::Buffer stagingBuffer;
					vk::DeviceMemory stagingMemory;

					vk::BufferCreateInfo bufCreateInfo = {};
					bufCreateInfo.size = tex2D.size();
					// This buffer is used as a transfer source for the buffer copy
					bufCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
					bufCreateInfo.sharingMode = vk::SharingMode::eExclusive;

					stagingBuffer = device.createBuffer(bufCreateInfo);

					// Get memory requirements for the staging buffer (alignment, memory type bits)
					memReqs = device.getBufferMemoryRequirements(stagingBuffer);

					memAllocInfo.allocationSize = memReqs.size;
					// Get memory type index for a host visible buffer
					memAllocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, physicalDevice.getMemoryProperties());

					stagingMemory = device.allocateMemory(memAllocInfo);
					device.bindBufferMemory(stagingBuffer, stagingMemory, 0);

					// Copy texture data into staging buffer
					uint8_t *data;
					data = reinterpret_cast<uint8_t*>(device.mapMemory(stagingMemory, 0, memReqs.size));
					memcpy(data, tex2D.data(), tex2D.size());
					device.unmapMemory(stagingMemory);

					// Setup buffer copy regions for each mip level
					std::vector<vk::BufferImageCopy> bufferCopyRegions;
					uint32_t offset = 0;

					for(uint32_t i = 0; i < mipLevels; i++)
					{
						vk::BufferImageCopy bufferCopyRegion = {};
						bufferCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
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
					vk::ImageCreateInfo imageCreateInfo = {};
					imageCreateInfo.imageType = vk::ImageType::e2D;
					imageCreateInfo.format = format;
					imageCreateInfo.mipLevels = mipLevels;
					imageCreateInfo.arrayLayers = 1;
					imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
					imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
					imageCreateInfo.usage = vk::ImageUsageFlagBits::eSampled;
					imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
					imageCreateInfo.initialLayout = vk::ImageLayout::ePreinitialized;
					imageCreateInfo.extent = vk::Extent3D(width, height, 1);
					imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;

					image = device.createImage(imageCreateInfo);

					memReqs = device.getImageMemoryRequirements(image);

					memAllocInfo.allocationSize = memReqs.size;

					memAllocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, physicalDevice.getMemoryProperties());
					deviceMemory = device.allocateMemory(memAllocInfo);
					device.bindImageMemory(image, deviceMemory, 0);

					vk::ImageSubresourceRange subresourceRange = vk::ImageSubresourceRange()
						.setAspectMask(vk::ImageAspectFlagBits::eColor)
						.setLevelCount(mipLevels)
						.setLayerCount(1);

					// Image barrier for optimal image (target)
					// Optimal image will be used as destination for the copy
					// Create an image barrier object
					vk::ImageMemoryBarrier imageMemoryBarrier = {};
					// Some default values
					imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageMemoryBarrier.oldLayout = vk::ImageLayout::ePreinitialized;
					imageMemoryBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
					imageMemoryBarrier.image = image;
					imageMemoryBarrier.subresourceRange = subresourceRange;
					imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
					imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

					// Put barrier on top
					vk::PipelineStageFlags srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
					vk::PipelineStageFlags destStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;

					// Put barrier inside setup command buffer
					cmdBuffer.pipelineBarrier(srcStageFlags, destStageFlags, vk::DependencyFlags(), nullptr, nullptr, imageMemoryBarrier);


					// Copy mip levels from staging buffer
					cmdBuffer.copyBufferToImage(stagingBuffer, image, vk::ImageLayout::eTransferDstOptimal, bufferCopyRegions);

					// Change texture image layout to shader read after all mip levels have been copied
					imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

					// Create an image barrier object
					vk::ImageMemoryBarrier imageMemoryBarrier2 = {};
					// Some default values
					imageMemoryBarrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageMemoryBarrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageMemoryBarrier2.oldLayout = vk::ImageLayout::eTransferDstOptimal;
					imageMemoryBarrier2.newLayout = imageLayout;
					imageMemoryBarrier2.image = image;
					imageMemoryBarrier2.subresourceRange = subresourceRange;
					imageMemoryBarrier2.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
					imageMemoryBarrier2.dstAccessMask = vk::AccessFlagBits::eShaderRead;
					// Put barrier on top
					vk::PipelineStageFlags srcStageFlags2 = vk::PipelineStageFlagBits::eTopOfPipe;
					vk::PipelineStageFlags destStageFlags2 = vk::PipelineStageFlagBits::eTopOfPipe;

					// Put barrier inside setup command buffer
					cmdBuffer.pipelineBarrier(srcStageFlags2, destStageFlags2, vk::DependencyFlags(), nullptr, nullptr, imageMemoryBarrier2);
					// Submit command buffer containing copy and image layout commands
					cmdBuffer.end();

					// Create a fence to make sure that the copies have finished before continuing
					vk::Fence copyFence;
					vk::FenceCreateInfo fenceCreateInfo = {};
					copyFence = device.createFence(fenceCreateInfo);

					vk::SubmitInfo submitInfo = {};
					submitInfo.commandBufferCount = 1;
					submitInfo.pCommandBuffers = &cmdBuffer;
					queue.submit(submitInfo, copyFence);
					device.waitForFences(copyFence, VK_TRUE, 1000000000000);

					device.destroyFence(copyFence);

					// Clean up staging resources
					device.freeMemory(stagingMemory);
					device.destroyBuffer(stagingBuffer);
				}
				else
				{
					// Prefer using optimal tiling, as linear tiling 
					// may support only a small set of features 
					// depending on implementation (e.g. no mip maps, only one layer, etc.)

					// Check if this support is supported for linear tiling
					assert(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

					vk::Image mappableImage;
					vk::DeviceMemory mappableMemory;

					vk::ImageCreateInfo imageCreateInfo = {};
					imageCreateInfo.imageType = vk::ImageType::e2D;
					imageCreateInfo.format = format;
					imageCreateInfo.extent = vk::Extent3D(width, height, 1);
					imageCreateInfo.mipLevels = 1;
					imageCreateInfo.arrayLayers = 1;
					imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
					imageCreateInfo.tiling = vk::ImageTiling::eLinear;
					imageCreateInfo.usage = (useStaging) ? vk::ImageUsageFlagBits::eTransferSrc : vk::ImageUsageFlagBits::eSampled;
					imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
					imageCreateInfo.initialLayout = vk::ImageLayout::ePreinitialized;

					// Load mip map level 0 to linear tiling image
					mappableImage = device.createImage(imageCreateInfo);

					// Get memory requirements for this image 
					// like size and alignment
					memReqs = device.getImageMemoryRequirements(mappableImage);
					// Set memory allocation size to required memory size
					memAllocInfo.allocationSize = memReqs.size;

					// Get memory type that can be mapped to host memory
					memAllocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, physicalDevice.getMemoryProperties());

					// Allocate host memory
					mappableMemory = device.allocateMemory(memAllocInfo);

					// Bind allocated image for use
					device.bindImageMemory(mappableImage, mappableMemory, 0);

					// Get sub resource layout
					// Mip map count, array layer, etc.
					vk::ImageSubresource subRes = {};
					vk::SubresourceLayout subResLayout;
					void *data;

					// Get sub resources layout 
					// Includes row pitch, size offsets, etc.
					subResLayout = device.getImageSubresourceLayout(mappableImage, subRes);

					// Map image memory
					data = device.mapMemory(mappableMemory, 0, memReqs.size);

					// Copy image data into memory
					memcpy(data, tex2D[subRes.mipLevel].data(), tex2D[subRes.mipLevel].size());

					device.unmapMemory(mappableMemory);

					// Linear tiled images don't need to be staged
					// and can be directly used as textures
					image = mappableImage;
					deviceMemory = mappableMemory;
					imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

					// Setup image memory barrier
					vk::ImageSubresourceRange subresourceRange = {};
					subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
					subresourceRange.baseMipLevel = 0;
					subresourceRange.levelCount = 1;
					subresourceRange.layerCount = 1;

					// Create an image barrier object
					vk::ImageMemoryBarrier imageMemoryBarrier = {};
					imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageMemoryBarrier.oldLayout = vk::ImageLayout::ePreinitialized;
					imageMemoryBarrier.newLayout = imageLayout;
					imageMemoryBarrier.image = image;
					imageMemoryBarrier.subresourceRange = subresourceRange;
					imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite;
					imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
					// Put barrier on top
					vk::PipelineStageFlags srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
					vk::PipelineStageFlags destStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;

					// Put barrier inside setup command buffer
					cmdBuffer.pipelineBarrier(srcStageFlags, destStageFlags, vk::DependencyFlags(), nullptr, nullptr, imageMemoryBarrier);

					// Submit command buffer containing copy and image layout commands
					cmdBuffer.end();

					vk::Fence nullFence = { VK_NULL_HANDLE };

					vk::SubmitInfo submitInfo = {};
					submitInfo.commandBufferCount = 1;
					submitInfo.pCommandBuffers = &cmdBuffer;

					queue.submit(submitInfo, nullFence);
					queue.waitIdle();
				}

				// Create sampler
				vk::SamplerCreateInfo sampler2 = {};
				sampler2.magFilter = vk::Filter::eLinear;
				sampler2.minFilter = vk::Filter::eLinear;
				sampler2.mipmapMode = vk::SamplerMipmapMode::eLinear;
				sampler2.addressModeU = vk::SamplerAddressMode::eRepeat;
				sampler2.addressModeV = vk::SamplerAddressMode::eRepeat;
				sampler2.addressModeW = vk::SamplerAddressMode::eRepeat;
				sampler2.mipLodBias = 0.0f;
				sampler2.compareOp = vk::CompareOp::eNever;
				sampler2.minLod = 0.0f;
				// Max level-of-detail should match mip level count
				sampler2.maxLod = (useStaging) ? (float) mipLevels : 0.0f;
				// Enable anisotropic filtering
				sampler2.maxAnisotropy = 8;
				sampler2.anisotropyEnable = VK_TRUE;
				sampler2.borderColor = vk::BorderColor::eFloatOpaqueWhite;
				sampler = device.createSampler(sampler2);

				// Create image view
				// Textures are not directly accessed by the shaders and
				// are abstracted by image views containing additional
				// information and sub resource ranges
				vk::ImageViewCreateInfo view2 = {};
				view2.viewType = vk::ImageViewType::e2D;
				view2.format = format;
				view2.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
				view2.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
				// Linear tiling usually won't support mip maps
				// Only set mip map count if optimal tiling is used
				view2.subresourceRange.levelCount = (useStaging) ? mipLevels : 1;
				view2.image = image;
				view = device.createImageView(view2);
			}


		};
	}
}