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
private:
	void initRenderPass(VkFormat depthStencilFormat, VkSurfaceFormatKHR surfaceFormat);
	void destroyRenderPass();

	Util* util = nullptr;
	Renderer* renderer = nullptr;

	VkRenderPass renderPass = nullptr;
};

