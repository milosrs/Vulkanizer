#pragma once
#ifndef RENDER_PASS_H
#define RENDER_PASS_H
#endif
#include "PLATFORM.h"
#include <array>
#include <vector>

class Renderer;

class RenderPass
{
public:
	RenderPass(VkSurfaceFormatKHR surfaceFormat, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
	~RenderPass();

	VkRenderPass getRenderPass();
	VkRenderPass* getRenderPassPTR();
	VkSampleCountFlagBits getSamples();

	void beginRenderPass(VkCommandBuffer, VkRenderPassBeginInfo*);
	void endRenderPass(VkCommandBuffer commandBuffer);
private:
	void initRenderPass();

	void createColorDepth();
	void createColor();

	Renderer* renderer = nullptr;

	VkRenderPass renderPass = nullptr;
	VkSampleCountFlagBits samples;
	VkFormat colorFormat;
	VkSurfaceFormatKHR surfaceFormat;

	VkSubpassDependency subpassDependency = {};
};

