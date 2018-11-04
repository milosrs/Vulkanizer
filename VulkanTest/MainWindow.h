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
	~MainWindow();

	void close();
	bool update();
	void continueInitialization();

	void beginRender();
	void endRender(std::vector<VkSemaphore>);
	void mainLoop();

	RenderPass getRenderPass();
	FrameBuffer getActiveFrameBuffer();
	Swapchain getSwapchain();

	VkSurfaceKHR getSurface();
	VkExtent2D getSurfaceSize();
	VkSurfaceKHR getSurfaceKHR();
	VkSurfaceCapabilitiesKHR getSurfaceCapatibilities();
	VkBool32 getIsWSISupported();
	VkSurfaceFormatKHR getSurfaceFormat();
private:
	void InitOSWindow();
	void DeinitOSWindow();
	void UpdateOSWindow();
	void InitOSSurface();

	void InitSurface();
	void DestroySurface();

	void initSync();
	void destroySync();

	Swapchain swapchain;
	RenderPass renderPass;
	FrameBuffer frameBuffer;

	Renderer* renderer = nullptr;
	GLFWwindow* window = nullptr;

	VkSurfaceKHR surfaceKHR = nullptr;
	VkSurfaceCapabilitiesKHR surfaceCapatibilities = {};
	VkBool32 isWSISupported = false;
	VkSurfaceFormatKHR surfaceFormat = {};
	
	uint32_t sizeX = 512;
	uint32_t sizeY = 512;
	std::string name = "MainWindow";

	bool window_should_run = true;
	uint32_t							surfaceX = 512;
	uint32_t							surfaceY = 512;
	std::string							_window_name;

	Util* util = nullptr;


};

