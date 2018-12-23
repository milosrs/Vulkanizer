#pragma once
#ifndef RENDER_PASS_H
#define RENDER_PASS_H
#endif
#include "PLATFORM.h"
#include "Renderer.h"
#include "Util.h"
#include <array>
#include <vector>

class Renderer;

class RenderPass
{
public:
	RenderPass(Renderer* renderer, VkFormat depthStencilFormat, VkSurfaceFormatKHR surfaceFormat);
	~RenderPass();

	VkRenderPass getRenderPass();
	VkRenderPass* getRenderPassPTR();

	void beginRenderPass(VkCommandBuffer, VkRenderPassBeginInfo*);
	void endRenderPass(VkCommandBuffer commandBuffer);
private:
	void initRenderPass();

	void createDepthStencil();
	void createColor();

	Renderer* renderer = nullptr;

	VkRenderPass renderPass = nullptr;
	
	VkFormat depthStencilFormat;
	VkFormat colorFormat;
	VkSurfaceFormatKHR surfaceFormat;

	VkSubpassDependency subpassDependency = {};
};

