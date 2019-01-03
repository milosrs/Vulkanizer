#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H
#endif
#include <cstring>
#include <memory>
#include "StagingBuffer.h"
#include "CommandBufferHandler.h"
#include "PLATFORM.h"
#include "Util.h"
#include <vector>

class Texture
{
public:
	/*String: Path to image,
	  Unsigned: Mode in which to open the image*/
	Texture(VkDevice, VkPhysicalDeviceMemoryProperties*, VkFormat, std::string, unsigned int);
	~Texture();

	void beginCreatingTexture(VkCommandPool, VkQueue);
	
	void supportsLinearBlitFormat(VkPhysicalDevice);

	VkSampler getSampler();
	VkImageView getTextureImageView();
	VkImage getTextureImage();
private:
	void createSampler();
	void generateMipmaps(VkCommandPool, VkQueue);

	int width, height, channelCount;
	unsigned int mode;
	unsigned char* pixels;
	unsigned int mipLevels;
	bool supportsLinearBlit;

	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties *physicalProperties = nullptr;
	VkFormat imageFormat = {};
	VkDeviceSize size = VK_NULL_HANDLE;
	VkImage texture = VK_NULL_HANDLE;
	VkImageView textureView = VK_NULL_HANDLE;
	VkDeviceMemory textureMemory = VK_NULL_HANDLE;
	VkSampler sampler = VK_NULL_HANDLE;

	std::vector<VkImage> mipmaps;

	StagingBuffer<unsigned char>* stagingBuffer = nullptr;
};

