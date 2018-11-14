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
	renderer = renderer;

	for (VkImageView imageView : imageViews) {
		initFrameBuffer(swapchainImageCount, imageViews, renderPass, attachments, surfaceSize);
	}
	
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
	std::vector<VkImageView> attachments,
	VkExtent2D surfaceSize
)
{
	frameBuffers.resize(swapchainImageCount);

	for (uint32_t i = 0; i < swapchainImageCount; ++i) {
		const int attachmentsArraySize = attachments.size() + 1;
		VkFramebufferCreateInfo frameBufferCreateInfo{};
		std::vector<VkImageView> allAttachments(attachmentsArraySize);

		allAttachments.insert(allAttachments.begin(), attachments.begin(), attachments.end());

		frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		frameBufferCreateInfo.renderPass = renderPass;
		frameBufferCreateInfo.width = surfaceSize.width;
		frameBufferCreateInfo.height = surfaceSize.height;
		frameBufferCreateInfo.layers = 1;
		frameBufferCreateInfo.pAttachments = allAttachments.data();
		frameBufferCreateInfo.attachmentCount = allAttachments.size();

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
