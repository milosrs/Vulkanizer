#pragma once
#include "PLATFORM.h"
#include "Util.h"
#include "Renderer.h"
#include <array>
#include <vector>

class Renderer;

class RenderPass
{
public:
	RenderPass(Renderer* renderer, VkFormat depthStencilFormat, VkSurfaceFormatKHR surfaceFormat);
	RenderPass();
	~RenderPass();

	VkRenderPass getRenderPass();
	VkRenderPass* getRenderPassPTR();

	void beginRenderPass(VkFramebuffer frameBuffer, VkExtent2D extent, VkCommandBuffer commandBuffer, std::array<VkClearValue, 2> clearValues);
	void endRenderPass(VkCommandBuffer commandBuffer);
private:
	void initRenderPass();

	void createDepthStencil();
	void createColor();

	Util* util = nullptr;
	Renderer* renderer = nullptr;

	VkRenderPass renderPass = nullptr;
	
	VkFormat depthStencilFormat;
	VkFormat colorFormat;
	VkSurfaceFormatKHR surfaceFormat;

	VkSubpassDependency subpassDependency = {};
};

