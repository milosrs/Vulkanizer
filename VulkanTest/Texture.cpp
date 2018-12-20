#include "pch.h"
#include "Texture.h"


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
	VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags imageMemoryProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	VkImageViewCreateInfo imgCreateInfo = {};

	util->createImage(width, height, imageFormat, tiling, usage,
		imageMemoryProps, &texture, &textureMemory, device,
		physicalProperties, size, pixels, stagingBuffer);

	VkCommandBuffer transitioner = CommandBufferHandler::createOneTimeUsageBuffer(commandPool, device);
	util->transitionImageLayout(texture, imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, transitioner);
	CommandBufferHandler::endOneTimeUsageBuffer(transitioner, queue, commandPool, device);
	util->copyBufferToimage(transitioner, stagingBuffer->getBuffer(), texture, width, height);

	VkCommandBuffer retransitioner = CommandBufferHandler::createOneTimeUsageBuffer(commandPool, device);
	util->transitionImageLayout(texture, imageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, transitioner); //Mozda treba napraviti novi command buffer za ovu operaciju.
	CommandBufferHandler::endOneTimeUsageBuffer(retransitioner, queue, commandPool, device);

	this->textureView = util->createImageView(device, texture, imageFormat);
}

void Texture::createSampler()
{
	VkSamplerCreateInfo info = {};

	info.sType - VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
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
	info.maxAnisotropy = 1;

	util->ErrorCheck(vkCreateSampler(device, &info, nullptr, &sampler));
}
