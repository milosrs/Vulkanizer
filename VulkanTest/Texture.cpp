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

	this->mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

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
	VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags imageMemoryProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	
	stagingBuffer->fillBuffer(pixels);
	stbi_image_free(pixels);

	Util::createImage(width, height, mipLevels, imageFormat, tiling, usage,
		imageMemoryProps, &texture, &textureMemory, device, physicalProperties);

	Util::transitionImageLayout(texture, imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, 
								VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandPool, queue, device, mipLevels);

	Util::copyBufferToimage(stagingBuffer->getBuffer(), &texture, width, height, commandPool, device, queue);

	Util::transitionImageLayout(texture, imageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
								VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandPool, queue, device, mipLevels);

	generateMipmaps(commandPool, queue);
	this->textureView = Util::createImageView(device, texture, imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
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
	info.maxLod = static_cast<uint32_t>(mipLevels);
	info.anisotropyEnable = VK_TRUE;						//Anisotropy je opciona, mora se navesti u logicalDevice-u
	info.maxAnisotropy = 16;

	Util::ErrorCheck(vkCreateSampler(device, &info, nullptr, &sampler));
}

void Texture::generateMipmaps(VkCommandPool cmdPool, VkQueue queue)
{
	int32_t mipWidth = width;
	int32_t mipHeight = height;
	VkCommandBuffer generator = CommandBufferHandler::createOneTimeUsageBuffer(cmdPool, device);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = texture;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	for (int i = 1; i < mipLevels; ++i) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(generator, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr, 0, nullptr, 1, &barrier);

		VkImageBlit blit = {};
		blit.srcOffsets[0] = {0, 0, 0};
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.srcSubresource.mipLevel = i - 1;
		blit.dstOffsets[0] = { 0,0,0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1,  mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;
		blit.dstSubresource.mipLevel = i;

		VkFilter filter;

		if (supportsLinearBlit) {
			filter = VK_FILTER_LINEAR;
		}
		else {
			filter = VK_FILTER_NEAREST;
		}

		vkCmdBlitImage(generator, texture, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit, filter);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(generator, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &barrier);

		if (mipWidth > 1)
			mipWidth /= 2;
		if (mipHeight > 1)
			mipHeight /= 2;
	} 


	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(generator, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0, 0, nullptr, 0, nullptr, 1, &barrier);

	CommandBufferHandler::endOneTimeUsageBuffer(generator, queue, cmdPool, device);
}

void Texture::supportsLinearBlitFormat(VkPhysicalDevice physicalDevice)
{
	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &props);

	supportsLinearBlit = props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
}
