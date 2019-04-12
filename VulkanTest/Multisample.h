#pragma once
#include "PLATFORM.h"

class Multisample
{
public:
	Multisample(VkDevice device, VkPhysicalDeviceMemoryProperties *memprops, VkCommandPool cmdPool, VkQueue queue,
		VkFormat format, uint32_t w, uint32_t h, VkSampleCountFlagBits samples);
	~Multisample();

	VkImageView getView();
private:
	VkFormat swapchainFormat;
	VkImage image;
	VkDeviceMemory imageMemory;
	VkImageView imageView;

	VkDevice device = VK_NULL_HANDLE;
};

