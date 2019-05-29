#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H
#endif
#include <cstring>
#include <memory>
#include <algorithm>
#include "PLATFORM.h"
#include <vector>

template <typename T> class StagingBuffer;

namespace vkglTF {
	struct TextureSampler;
};

class Texture
{
public:
	/*String: Path to image,
	  Unsigned: Mode in which to open the image*/
	Texture(VkDevice, VkPhysicalDeviceMemoryProperties*, VkFormat, std::string, unsigned int, 
		VkSamplerCreateInfo *samplerInfo = nullptr, vkglTF::TextureSampler *glTFsampler = nullptr);
	Texture(VkDevice, VkPhysicalDeviceMemoryProperties*, VkFormat, unsigned char*, size_t width, size_t height);
	~Texture();

	void beginCreatingTexture(VkCommandPool, VkQueue, bool isCubemap = false);
	static void readImage(std::string path, unsigned char* buffer, int* w, int *h, int *channels, unsigned int mode);
	void supportsLinearBlitFormat(VkPhysicalDevice);

	VkSampler getSampler();
	VkImageView getTextureImageView();
	VkImage getTextureImage();
	std::string getTextureId();
	TextureType getTextureType();

	void setTextureId(std::string id);
	void setTextureType(std::string textureName = "");
	unsigned int getFullDimension();
	unsigned char* getPixels();
private:
	void createSampler(VkSamplerCreateInfo *samplerInfo = nullptr, vkglTF::TextureSampler *glTFsampler = nullptr);
	void generateMipmaps(VkCommandPool, VkQueue);

	int width, height, channelCount;
	unsigned int mode;
	unsigned char* pixels;
	uint32_t mipLevels;
	bool supportsLinearBlit;
	std::string textureId;
	TextureType type;

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

