#pragma once
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#endif // !MAIN_WINDOW_H

#include "PLATFORM.h"
#include "BUILD_OPTIONS.h"
#include "Util.h"
#include "Util.h"
#include "Swapchain.h"
#include "RenderPass.h"
#include "FrameBuffer.h"
#include "Pipeline.h"
#include "QueueFamilyIndices.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>
#include <vector>
#include <array>

class Renderer;

class MainWindow
{
public:
	MainWindow(Renderer* renderer, uint32_t sizeX, uint32_t sizeY, std::string windowName);
	MainWindow(const MainWindow&);
	~MainWindow();

	void continueInitialization(Renderer* renderer);

	void beginRender(VkSemaphore semaphoreToWait);
	void endRender(std::vector<VkSemaphore>);

	void recreateSwapchain();

	Renderer* getRenderer();
	RenderPass* getRenderPass();
	FrameBuffer* getActiveFrameBuffer();
	Swapchain* getSwapchain();
	GLFWwindow* getWindowPTR();
	Pipeline* getPipelinePTR();
	std::vector< CommandBuffer*> getCommandBuffers();
	CommandPool* getCommandPoolPTR();

	VkSurfaceKHR getSurface();
	VkSurfaceKHR* getSurfacePTR();
	VkExtent2D getSurfaceSize();
	VkSurfaceCapabilitiesKHR getSurfaceCapatibilities();
	VkBool32 getIsWSISupported();
	VkSurfaceFormatKHR getSurfaceFormat();
private:
	void InitOSWindow();
	void DeinitOSWindow();
	void InitOSSurface();

	void InitSurface();
	void DestroySurface();

	void choosePreferedFormat();
	void destroySwapchainDependencies();

	std::unique_ptr<Swapchain> swapchain = nullptr;
	std::unique_ptr<RenderPass> renderPass = nullptr;
	std::unique_ptr<FrameBuffer> frameBuffer = nullptr;
	std::unique_ptr<Pipeline> pipeline = nullptr;
	std::unique_ptr<CommandPool> cmdPool = nullptr;
	std::vector<CommandBuffer*> cmdBuffers;

	Renderer* renderer = nullptr;
	GLFWwindow* window = nullptr;
	Util* util = nullptr;

	VkSurfaceKHR surfaceKHR = VK_NULL_HANDLE;
	VkSurfaceCapabilitiesKHR surfaceCapatibilities = {};
	VkBool32 isWSISupported = false;
	VkSurfaceFormatKHR surfaceFormat = {};
	VkPresentInfoKHR presentInfo{};

	uint32_t sizeX;
	uint32_t sizeY;
	std::string name = "MainWindow";

	bool window_should_run = true;
};

