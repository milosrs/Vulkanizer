#pragma once
#include "PLATFORM.h"
#include "Util.h"
class DepthTester
{
public:
	DepthTester(VkDevice, VkPhysicalDevice, VkPhysicalDeviceMemoryProperties);
	~DepthTester();

	void createDepthImage(uint32_t, uint32_t, VkCommandPool, VkQueue, VkSampleCountFlagBits);
	VkImageView getDepthImageView();
	VkImage getDepthImage();

	static VkFormat findFormat(VkDevice, VkPhysicalDevice);
	static bool isInstanceCreated();
private:
	VkImage depth;
	VkImageView depthView;
	VkDeviceMemory depthMemory;
	VkFormat depthFormat;

	VkDevice device;
	VkPhysicalDevice gpu;
	VkPhysicalDeviceMemoryProperties memprops;
};

