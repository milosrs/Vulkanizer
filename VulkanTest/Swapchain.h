#pragma once
#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H
#endif // !MAIN_WINDOW_H
#include "PLATFORM.h"
#include "Renderer.h"
#include <vector>
#include <algorithm>

class MainWindow;
class Renderer;

class Swapchain
{
public:
	Swapchain(MainWindow*, Renderer*);
	Swapchain();
	~Swapchain();

	VkSwapchainKHR getSwapchain();
	VkSwapchainKHR* getSwapchainPTR();
	uint32_t* getActiveImageSwapchainPTR();
	uint32_t getActiveImageSwapchain();
	VkFormat getDepthStencilFormat();
	uint32_t getSwapchainImageCount();
	VkImageView getDepthStencilImageView();
	std::vector<VkImageView> getImageViews();
private:
	void initSwapchain();
	void destroySwapchain();

	void initSwapchainImgs();
	void destroySwapchainImgs();

	void initDepthStencilImage();
	void destroyDepthStencilImage();

	void setupSwapExtent();

	VkPresentModeKHR getAvaiablePresentMode();
	bool isStencilAvaiable();

	MainWindow* mainWindow = nullptr;
	Renderer* renderer = nullptr;
	Util* util = nullptr;

	VkBool32 isWSISupported = false;

	bool stencilAvaiable = false;
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	VkImage depthStencilImage = VK_NULL_HANDLE;						//Ne treba nam vise od jedne slike, jedino ako hocemo double/tripple buffering
	VkImageView depthStencilImageView = VK_NULL_HANDLE;				//Ne treba nam vise od jedne slike, jedino ako hocemo double/tripple buffering
	VkFormat depthStencilFormat = VK_FORMAT_UNDEFINED;
	VkMemoryAllocateInfo allocateInfo = {};
	VkDeviceMemory depthStencilImageMemory = VK_NULL_HANDLE;
	VkExtent2D swapExtent = {};
	VkFormat imagesFormat = VK_FORMAT_UNDEFINED;

	uint32_t swapchainImageCount = 2;
	uint32_t activeImageSwapchainId;
	uint32_t sizeX = 512;
	uint32_t sizeY = 512;
};

