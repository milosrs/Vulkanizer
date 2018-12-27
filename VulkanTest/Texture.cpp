#include "pch.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(VkDevice device, VkPhysicalDeviceMemoryProperties *memprops, VkFormat imageFormat,
				std::string path, unsigned int mode)
{
	this->device = device;
	this->imageFormat = imageFormat;
	this->physicalProperties = memprops;
	
	pixels = stbi_load(path.c_str(), &width, &height, &channelCount, mode);
	size = width * height * mode;													//Mode je ustvari broj bajtova po pikselu

	if (!pixels) {
		throw std::runtime_error("Error opening texture file: " + path);
		exit(1);
	}

	stagingBuffer = new StagingBuffer<unsigned char>(device, *memprops, size);
	createSampler();
}

Texture::~Texture()
{
	vkDestroySampler(device, sampler, nullptr);
	vkDestroyImageView(device, this->textureView, nullptr);
	vkDestroyImage(device, texture, nullptr);
	vkFreeMemory(device, textureMemory, nullptr);
}

void Texture::beginCreatingTexture(VkCommandPool commandPool, VkQueue queue)
{
	VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
	VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	VkMemoryPropertyFlags imageMemoryProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	
	stagingBuffer->fillBuffer(pixels);
	stbi_image_free(pixels);

	Util::createImage(width, height, imageFormat, tiling, usage,
		imageMemoryProps, &texture, &textureMemory, device, physicalProperties);

	Util::transitionImageLayout(&texture, imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, 
								VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandPool, queue, device);

	Util::copyBufferToimage(stagingBuffer->getBuffer(), &texture, width, height, commandPool, device, queue);

	Util::transitionImageLayout(&texture, imageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
								VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandPool, queue, device);

	this->textureView = Util::createImageView(device, texture, imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
}

VkSampler Texture::getSampler()
{
	return sampler;
}

VkImageView Texture::getTextureImageView()
{
	return textureView;
}

VkImage Texture::getTextureImage()
{
	return texture;
}

void Texture::createSampler()
{
	VkSamplerCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	info.minFilter = VK_FILTER_LINEAR;						//Undersampler
	info.magFilter = VK_FILTER_LINEAR;						//Oversampler
	info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;		//U, V, W su koordinate texela, za geometriju su x,y,z
	info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	info.anisotropyEnable = VK_TRUE;
	info.maxAnisotropy = 16;
	info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	info.compareEnable = VK_FALSE;
	info.compareOp = VK_COMPARE_OP_ALWAYS;
	info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	info.mipLodBias = 0.0f;									//Level of details bias.
	info.minLod = 0.0f;
	info.maxLod = 0.0f;
	info.anisotropyEnable = VK_TRUE;						//Anisotropy je opciona, mora se navesti u logicalDevice-u
	info.maxAnisotropy = 16;

	Util::ErrorCheck(vkCreateSampler(device, &info, nullptr, &sampler));
}
