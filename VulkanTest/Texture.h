#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cstring>
#include <memory>
#include "StagingBuffer.h"
#include "PLATFORM.h"
#include "Util.h"

class Texture
{
public:
	/*String: Path to image,
	  Unsigned: Mode in which to open the image*/
	Texture(VkDevice, VkPhysicalDeviceMemoryProperties*, VkFormat, std::string, unsigned int);
	~Texture();

	void beginCreatingTexture(VkCommandPool, VkQueue);
private:
	void createSampler();

	int width, height, channelCount;
	unsigned int mode;
	stbi_uc* pixels;

	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties *physicalProperties = nullptr;
	VkFormat imageFormat = {};
	VkDeviceSize size = VK_NULL_HANDLE;
	VkImage texture = VK_NULL_HANDLE;
	VkImageView textureView = VK_NULL_HANDLE;
	VkDeviceMemory textureMemory = VK_NULL_HANDLE;
	VkSampler sampler = VK_NULL_HANDLE;

	StagingBuffer<stbi_uc*>* stagingBuffer = nullptr;
	Util* util = &Util::instance();
};

