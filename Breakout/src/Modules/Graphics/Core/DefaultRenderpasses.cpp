#include "stdafx.h"
#include "Modules/Graphics/Core/DefaultRenderpasses.h"

namespace speck::graphics
{
	vk::RenderPass get_clear_pass(::graphics::VulkanContext& context, vk::Format& depthFormat, vk::Format& colorFormat)
	{
		// Attachments
		// Color attachment
		std::vector<vk::AttachmentDescription> attachments(2);
		attachments[0]
			.setFormat(colorFormat)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

		// Depth attachment
		attachments[1]
			.setFormat(depthFormat)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		// Subpasses
		std::vector<vk::AttachmentReference> colorReference(1);
		// Color reference
		colorReference[0]
			.setAttachment(0)
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

		// Depth reference
		vk::AttachmentReference depthReference;
		depthReference
			.setAttachment(1)
			.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		std::vector<vk::SubpassDescription> subpass(1);
		subpass[0]
			.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachmentCount(1)
			.setPColorAttachments(colorReference.data())
			.setPDepthStencilAttachment(&depthReference);

		// RenderPass creation
		vk::RenderPassCreateInfo renderPassInfo;
		renderPassInfo
			.setAttachmentCount(2)
			.setPAttachments(attachments.data())
			.setSubpassCount(1)
			.setPSubpasses(subpass.data());

		return context.device.createRenderPass(renderPassInfo);
	}

	vk::RenderPass get_draw_pass(::graphics::VulkanContext& context, vk::Format& depthFormat, vk::Format& colorFormat)
	{
		// Attachments
		// Color attachment
		std::vector<vk::AttachmentDescription> attachments(2);
		attachments[0]
			.setFormat(colorFormat)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eLoad)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

		// Depth attachment
		attachments[1]
			.setFormat(depthFormat)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eLoad)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		// Subpasses
		std::vector<vk::AttachmentReference> colorReference(1);
		// Color reference
		colorReference[0]
			.setAttachment(0)
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

		// Depth reference
		vk::AttachmentReference depthReference;
		depthReference
			.setAttachment(1)
			.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		std::vector<vk::SubpassDescription> subpass(1);
		subpass[0]
			.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachmentCount(1)
			.setPColorAttachments(colorReference.data())
			.setPDepthStencilAttachment(&depthReference);

		// RenderPass creation
		vk::RenderPassCreateInfo renderPassInfo;
		renderPassInfo
			.setAttachmentCount(2)
			.setPAttachments(attachments.data())
			.setSubpassCount(1)
			.setPSubpasses(subpass.data());

		return context.device.createRenderPass(renderPassInfo);
	}
}