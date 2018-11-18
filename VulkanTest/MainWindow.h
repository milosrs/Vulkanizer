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
#include "QueueFamilyIndices.h"
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

	void close();
	void continueInitialization(Renderer* renderer);

	void beginRender();
	void endRender(std::vector<VkSemaphore>);
	void mainLoop();

	Renderer* getRenderer();
	RenderPass* getRenderPass();
	FrameBuffer* getActiveFrameBuffer();
	Swapchain* getSwapchain();
	GLFWwindow* getWindowPTR();

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

	void initSync();
	void destroySync();

	void choosePreferedFormat();

	std::unique_ptr<Swapchain> swapchain = nullptr;
	std::unique_ptr<RenderPass> renderPass = nullptr;
	std::unique_ptr<FrameBuffer> frameBuffer = nullptr;

	Renderer* renderer = nullptr;
	GLFWwindow* window = nullptr;
	Util* util = nullptr;

	VkSurfaceKHR surfaceKHR = VK_NULL_HANDLE;
	VkSurfaceCapabilitiesKHR surfaceCapatibilities = {};
	VkBool32 isWSISupported = false;
	VkSurfaceFormatKHR surfaceFormat = {};
	
	uint32_t sizeX;
	uint32_t sizeY;
	std::string name = "MainWindow";

	bool window_should_run = true;
};

