#include "pch.h"
#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(
	Renderer* renderer,
	uint32_t swapchainImageCount,
	std::vector<VkImageView> imageViews,
	VkRenderPass renderPass,
	VkExtent2D surfaceSize,
	std::vector<VkImageView> attachments)
{
	util = &Util::instance();
	this->renderer = renderer;

	initFrameBuffer(swapchainImageCount, imageViews, renderPass, surfaceSize, attachments);
}

FrameBuffer::FrameBuffer()
{
}


FrameBuffer::~FrameBuffer()
{
	for (const VkFramebuffer& frameBuffer : frameBuffers) {
		vkDestroyFramebuffer(renderer->getDevice(), frameBuffer, nullptr);
	}

	frameBuffers.clear();
}

void FrameBuffer::initFrameBuffer(
	uint32_t swapchainImageCount,
	std::vector<VkImageView> imageViews,
	VkRenderPass renderPass,
	VkExtent2D surfaceSize,
	std::vector<VkImageView> attachments
)
{
	frameBuffers.resize(swapchainImageCount);

	for (uint32_t i = 0; i < swapchainImageCount; ++i) {
		
		VkFramebufferCreateInfo frameBufferCreateInfo{};
		std::vector<VkImageView> allAttachments = { imageViews[i] };

		allAttachments.insert(allAttachments.begin(), attachments.begin(), attachments.end());

		frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferCreateInfo.renderPass = renderPass;
		frameBufferCreateInfo.width = surfaceSize.width;
		frameBufferCreateInfo.height = surfaceSize.height;
		frameBufferCreateInfo.layers = 1;
		frameBufferCreateInfo.pAttachments = allAttachments.data();
		frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(allAttachments.size());

		util->ErrorCheck(vkCreateFramebuffer(renderer->getDevice(), &frameBufferCreateInfo, nullptr, &frameBuffers[i]));
	}
}

VkFramebuffer FrameBuffer::getActiveFrameBuffer(uint32_t activeImageSwapchainId)
{
	return this->frameBuffers[activeImageSwapchainId];
}

VkFence FrameBuffer::getActiveImageFence()
{
	return this->activeImageAvaiableFence;
}

VkFence * FrameBuffer::getActiveImageFencePTR()
{
	return &this->activeImageAvaiableFence;
}
