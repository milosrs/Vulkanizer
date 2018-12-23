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

class Texture
{
public:
	/*String: Path to image,
	  Unsigned: Mode in which to open the image*/
	Texture(VkDevice, VkPhysicalDeviceMemoryProperties*, VkFormat, std::string, unsigned int);
	~Texture();

	void beginCreatingTexture(VkCommandPool, VkQueue);

	VkSampler getSampler();
	VkImageView getTextureImageView();
private:
	void createSampler();

	int width, height, channelCount;
	unsigned int mode;
	unsigned char* pixels;

	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties *physicalProperties = nullptr;
	VkFormat imageFormat = {};
	VkDeviceSize size = VK_NULL_HANDLE;
	VkImage texture = VK_NULL_HANDLE;
	VkImageView textureView = VK_NULL_HANDLE;
	VkDeviceMemory textureMemory = VK_NULL_HANDLE;
	VkSampler sampler = VK_NULL_HANDLE;

	StagingBuffer<unsigned char>* stagingBuffer = nullptr;
};

