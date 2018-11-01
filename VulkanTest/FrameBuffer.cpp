#include "pch.h"
#include "FrameBuffer.h"


FrameBuffer::FrameBuffer(
	Renderer* renderer,
	uint32_t swapchainImageCount,
	VkImageView depthStencilImageView,
	std::vector<VkImageView> imageViews,
	VkRenderPass renderPass,
	VkExtent2D surfaceSize)
{
	util = &Util::instance();
	renderer = renderer;
}

FrameBuffer::FrameBuffer()
{
}


FrameBuffer::~FrameBuffer()
{
	destroyFrameBuffer();
}

void FrameBuffer::initFrameBuffer(
	uint32_t swapchainImageCount,
	VkImageView depthStencilImageView,
	std::vector<VkImageView> imageViews,
	VkRenderPass renderPass,
	VkExtent2D surfaceSize
)
{
	frameBuffers.resize(swapchainImageCount);

	for (uint32_t i = 0; i < swapchainImageCount; ++i) {
		VkFramebufferCreateInfo frameBufferCreateInfo{};
		std::array<VkImageView, 2> attachments{};

		attachments[0] = depthStencilImageView;
		attachments[1] = imageViews[i];

		frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		frameBufferCreateInfo.renderPass = renderPass;
		frameBufferCreateInfo.width = surfaceSize.width;
		frameBufferCreateInfo.height = surfaceSize.height;
		frameBufferCreateInfo.layers = 1;
		frameBufferCreateInfo.pAttachments = attachments.data();
		frameBufferCreateInfo.attachmentCount = attachments.size();

		util->ErrorCheck(vkCreateFramebuffer(renderer->getDevice(), &frameBufferCreateInfo, nullptr, &frameBuffers[i]));
	}
}

void FrameBuffer::destroyFrameBuffer()
{
	for (const VkFramebuffer& frameBuffer : frameBuffers) {
		vkDestroyFramebuffer(renderer->getDevice(), frameBuffer, nullptr);
	}

	frameBuffers.clear();
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
