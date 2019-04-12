#pragma once
#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H
#endif
#include "PLATFORM.h"
#include <vector>
#include <array>

class MainWindow;
class DepthTester;
class Renderer;

class FrameBuffer
{
public:
	FrameBuffer
	(
		uint32_t swapchainImageCount,
		std::vector<VkImageView> imageViews,
		VkRenderPass renderPass,
		VkExtent2D surfaceSize,
		std::vector<VkImageView> attachments = {}
	);
	~FrameBuffer();

	VkFramebuffer getActiveFrameBuffer(uint32_t activeImageSwapchainId);
	VkFence getActiveImageFence();
	VkFence* getActiveImageFencePTR();
	std::vector<VkFramebuffer> getFrameBuffers();
private:
	void initFrameBuffer
	(
		uint32_t swapchainImageCount,
		std::vector<VkImageView> imageViews,
		VkRenderPass renderPass,
		VkExtent2D surfaceSize,
		std::vector<VkImageView> attachments = {}
	);

	std::vector<VkFramebuffer> frameBuffers;
	VkFence activeImageAvaiableFence = VK_NULL_HANDLE;

	Renderer* renderer = nullptr;
};

