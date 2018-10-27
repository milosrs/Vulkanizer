#pragma once
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#endif // !MAIN_WINDOW_H

#include "PLATFORM.h"
#include "BUILD_OPTIONS.h"
#include "Util.h"
#include "Util.h"
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>
#include <vector>

class Renderer;

class MainWindow
{
public:
	MainWindow(Renderer* renderer, uint32_t sizeX, uint32_t sizeY, std::string windowName);
	~MainWindow();

	void close();
	bool update();

private:
	void InitOSWindow();
	void DeinitOSWindow();
	void UpdateOSWindow();
	void InitOSSurface();

	void InitSurface();
	void DestroySurface();

	void initSwapchain();
	void destroySwapchain();

	void initSwapchainImgs();
	void destroySwapchainImgs();

	void initDepthStencilImage();
	void destroyDepthStencilImage();

	Renderer* renderer = NULL;
	VkSurfaceKHR surfaceKHR = nullptr;
	VkSurfaceCapabilitiesKHR surfaceCapatibilities = {};
	VkBool32 isWSISupported = false;
	VkSurfaceFormatKHR surfaceFormat = {};

	VkSwapchainKHR swapchain = nullptr;
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;

	VkImage depthStencilImage = nullptr;						//Ne treba nam vise od jedne slike, jedino ako hocemo double/tripple buffering
	VkImageView depthStencilImageView = nullptr;				//Ne treba nam vise od jedne slike, jedino ako hocemo double/tripple buffering
	VkFormat depthStencilFormat = VK_FORMAT_UNDEFINED;
	VkMemoryAllocateInfo allocateInfo = {};
	VkDeviceMemory depthStencilImageMemory = nullptr;

	uint32_t swapchainImageCount = 2;
	uint32_t sizeX = 512;
	uint32_t sizeY = 512;
	std::string name = "MainWindow";

	bool window_should_run = true;
	bool stencilAvaiable = false;
	uint32_t							surfaceX = 512;
	uint32_t							surfaceY = 512;
	std::string							_window_name;

	Util* util = nullptr;

#if VK_USE_PLATFORM_WIN32_KHR
	HINSTANCE							win32_instance = NULL;
	HWND								win32_window = NULL;
	std::string							win32_class_name;
	static uint64_t						win32_class_id_counter;
#elif VK_USE_PLATFORM_XCB_KHR
	xcb_connection_t				*	xcb_connection = nullptr;
	xcb_screen_t					*	xcb_screen = nullptr;
	xcb_window_t						xcb_window = 0;
	xcb_intern_atom_reply_t			*	xcb_atom_window_reply = nullptr;
#endif
};

