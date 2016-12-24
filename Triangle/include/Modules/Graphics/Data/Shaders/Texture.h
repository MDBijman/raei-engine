#pragma once
#include <vulkan\VULKAN.HPP>

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
			}

			void upload(vk::Device& device, vk::PhysicalDevice& physicalDevice)
			{
			}

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
			static uint32_t getMemoryType(uint32_t typeBits, VkFlags properties, VkPhysicalDeviceMemoryProperties deviceMemoryProperties);

			void load(const char* filename, vk::Format format, vk::PhysicalDevice& physicalDevice, vk::Device& device, vk::CommandPool& pool, vk::Queue& queue);
		};
	}
}