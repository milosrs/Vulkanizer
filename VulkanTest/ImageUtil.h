#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "PLATFORM.h"
#include "StagingBuffer.h"
class ImageUtil
{
public:
	ImageUtil();
	~ImageUtil();

	static void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory,
		VkDevice device, VkPhysicalDeviceMemoryProperties *memprops, VkDeviceSize size, stbi_uc* pixels);

	static void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

private:
};

