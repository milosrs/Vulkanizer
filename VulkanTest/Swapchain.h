#pragma once
#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H
#endif // !MAIN_WINDOW_H
#include "PLATFORM.h"
#include <vector>
#include <algorithm>
#include <Magick++.h>

class WindowController;
class MainWindow;
class Renderer;

class Swapchain
{
public:
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

	bool isSupportingBlit();

	void saveScreenshot(std::string filePath);
private:
	MainWindow *mainWindow = nullptr;
	Renderer *renderer = nullptr;

	void initSwapchain();
	void initSwapchainImgs();
	void setupScreenshotData();

	void setupSwapExtent();

	void cleanup();

	VkPresentModeKHR getAvaiablePresentMode();
	bool isStencilAvaiable();

	VkBool32 isWSISupported = false;

	bool stencilAvaiable = false;
	bool supportsBlit;
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	VkImage depthStencilImage = VK_NULL_HANDLE;						//Ne treba nam vise od jedne slike, jedino ako hocemo double/tripple buffering
	VkImageView depthStencilImageView = VK_NULL_HANDLE;				//Ne treba nam vise od jedne slike, jedino ako hocemo double/tripple buffering
	VkMemoryAllocateInfo allocateInfo = {};
	VkDeviceMemory depthStencilImageMemory = VK_NULL_HANDLE;
	VkExtent2D swapExtent = {};
	VkFormat imagesFormat = VK_FORMAT_UNDEFINED;
	VkFormat depthStencilFormat = VK_FORMAT_UNDEFINED;

	uint32_t swapchainImageCount = 2;
	uint32_t activeImageSwapchainId;
	uint32_t sizeX;
	uint32_t sizeY;

	const std::string magickPath = "C:\\Program Files\\ImageMagick-7.0.8-Q16";
};

