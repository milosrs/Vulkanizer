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
	Texture(std::string, unsigned int);
	~Texture();

	void createTextureTest(VkDevice, VkPhysicalDeviceMemoryProperties*, VkFormat);
private:

	int width, height, channelCount;
	unsigned int mode;
	stbi_uc* pixels;

	VkDeviceSize size = VK_NULL_HANDLE;
	VkImage texture = VK_NULL_HANDLE;
	VkDeviceMemory textureMemory = VK_NULL_HANDLE;

	std::unique_ptr<StagingBuffer<stbi_uc*>> stagingBuffer = nullptr;
	Util* util = &Util::instance();
};

