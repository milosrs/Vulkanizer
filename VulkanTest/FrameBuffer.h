#pragma once
#include "PLATFORM.h"
#include "Renderer.h"
#include <vector>
#include <array>

class Renderer;

class FrameBuffer
{
public:
	FrameBuffer
	(
		Renderer* renderer,
		uint32_t swapchainImageCount,
		VkImageView depthStencilImageView,
		std::vector<VkImageView> imageViews,
		VkRenderPass renderPass,
		VkExtent2D surfaceSize
	);
	FrameBuffer();
	~FrameBuffer();

	VkFramebuffer getActiveFrameBuffer(uint32_t activeImageSwapchainId);
	VkFence getActiveImageFence();
	VkFence* getActiveImageFencePTR();
private:
	void initFrameBuffer
	(
		uint32_t swapchainImageCount,
		VkImageView depthStencilImageView,
		std::vector<VkImageView> imageViews,
		VkRenderPass renderPass,
		VkExtent2D surfaceSize
	);
	void destroyFrameBuffer();

	std::vector<VkFramebuffer> frameBuffers;
	VkFence activeImageAvaiableFence = nullptr;

	Renderer* renderer = nullptr;
	Util* util = nullptr;
};

