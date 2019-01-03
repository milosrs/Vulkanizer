#include "pch.h"
#include "Util.h"
#include "StagingBuffer.h"
#include "CommandBufferHandler.h"

Util::Util()
{
}

void Util::printFPS()
{
	++frameCounter;

	if (last_time + std::chrono::seconds(1) < timer.now()) {
		last_time = timer.now();
		fps = frameCounter;
		frameCounter = 0;
		std::cout << "FPS: " + fps << std::endl;
	}
}

Util::~Util()
{
}

#if BUILD_ENABLE_VULKAN_RUNTIME_DEBUG

void Util::ErrorCheck(VkResult result) {
	if (result < 0) {
		std::cout << "Vulkan Error: ";
		switch (result) {
		case VK_ERROR_OUT_OF_HOST_MEMORY: std::cout << "Out of host memory" << std::endl; break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY : std::cout << "Out of device memory" << std::endl; break;
		case VK_ERROR_INITIALIZATION_FAILED : std::cout << "Initialization failed" << std::endl; break;
		case VK_ERROR_DEVICE_LOST : std::cout << "Device lost" << std::endl; break;
		case VK_ERROR_MEMORY_MAP_FAILED : std::cout << "Memory map failed" << std::endl; break;
		case VK_ERROR_LAYER_NOT_PRESENT : std::cout << "Layer not present" << std::endl; break;
		case VK_ERROR_EXTENSION_NOT_PRESENT :  std::cout << "Extension not present" << std::endl; break;
		case VK_ERROR_FEATURE_NOT_PRESENT : std::cout << "Feature not present" << std::endl; break;
		case VK_ERROR_INCOMPATIBLE_DRIVER : std::cout << "Incompatible driver" << std::endl; break;
		case VK_ERROR_TOO_MANY_OBJECTS : std::cout << "Too many objects" << std::endl; break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED : std::cout << "Format not supported" << std::endl; break;
		case VK_ERROR_FRAGMENTED_POOL : std::cout << "Fragmented pool" << std::endl; break;
		case VK_ERROR_OUT_OF_POOL_MEMORY : std::cout << "Out of pool memory" << std::endl; break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE : std::cout << "Invalid external handle" << std::endl; break;
		case VK_ERROR_SURFACE_LOST_KHR: std::cout << "Surface lost" << std::endl; break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR : std::cout << "Native window in use" << std::endl; break;
		case VK_ERROR_OUT_OF_DATE_KHR: std::cout << "Out of host date" << std::endl; break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR : std::cout << "Incompatible display" << std::endl; break;
		case VK_ERROR_VALIDATION_FAILED_EXT : std::cout << "Validation failed" << std::endl; break;
		case VK_ERROR_INVALID_SHADER_NV: std::cout << "Invalid shader" << std::endl; break;
		case VK_ERROR_FRAGMENTATION_EXT : std::cout << "Fragmentation error" << std::endl; break;
		case VK_ERROR_NOT_PERMITTED_EXT : std::cout << "Not permitted" << std::endl; break;
		}
		exit(-1);
	}
}

bool Util::hasStencilComponent(VkFormat depthFormat)
{
		return depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT ||
			depthFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
			depthFormat == VK_FORMAT_D16_UNORM_S8_UINT;
}

bool Util::shouldCreateDepthStencil()
{
	return selectedOption > 1;
}

void Util::setOption(int option)
{
	selectedOption = option;
}

VkDevice Util::getDevice()
{
	return device;
}

#else
void Util::ErrorCheck(VkResult result) {};
#endif

VkFormat Util::findSupportedFormat(VkPhysicalDevice physicalDevice, VkDevice device, const std::vector<VkFormat> &candidates,
									VkImageTiling tiling, VkFormatFeatureFlags features)
{
	VkFormat supportedFormat = {};

	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			supportedFormat = format;
		}
		else if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			supportedFormat = format;
		}
	}

	if (supportedFormat == VK_NULL_HANDLE) {
		throw new std::runtime_error("No supported format found.");
	}

	return supportedFormat;
}

wchar_t* Util::convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

uint32_t Util::findMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties * memoryProps, const VkMemoryRequirements * memoryRequirements, const VkMemoryPropertyFlags memoryFlags)
{
	uint32_t ret = NULL;

	for (uint32_t i = 0; i < memoryProps->memoryTypeCount; ++i) {
		if (memoryRequirements->memoryTypeBits & (1 << i)) {
			if ((memoryProps->memoryTypes[i].propertyFlags & memoryFlags) == memoryFlags) {
				ret = i;
			}
		}
	}

	return ret;
}

void Util::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
						VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory,
						VkDevice device, VkPhysicalDeviceMemoryProperties *memprops) {

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(device, &imageInfo, nullptr, image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, *image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryTypeIndex(memprops, &memRequirements, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device, *image, *imageMemory, 0);
}

void Util::transitionImageLayout(VkImage *image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, 
								VkCommandPool commandPool, VkQueue queue, VkDevice device, uint32_t mipLevels)
{
	VkCommandBuffer cmdBuffer = CommandBufferHandler::createOneTimeUsageBuffer(commandPool, device);
	VkImageMemoryBarrier barrier = {};
	VkPipelineStageFlags sourceStage = {};
	VkPipelineStageFlags dstStage = {};

	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = *image;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	/*Ova dva polja za redove moraju biti namestena da budu ignorisana,
		ako necemo da prebacujemo vlasnistvo nad slikom iz jednog Queue-a u drugi.*/

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (hasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(cmdBuffer, sourceStage, dstStage, 0, 0, nullptr, 0, nullptr, 0, &barrier);
	CommandBufferHandler::endOneTimeUsageBuffer(cmdBuffer, queue, commandPool, device);
}

void Util::copyBufferToimage(VkBuffer buffer, VkImage *image, uint32_t width, uint32_t height,
								VkCommandPool commandPool, VkDevice device, VkQueue queue)
{
	VkCommandBuffer cmdBuffer = CommandBufferHandler::createOneTimeUsageBuffer(commandPool, device);

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(cmdBuffer, buffer, *image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	CommandBufferHandler::endOneTimeUsageBuffer(cmdBuffer, queue, commandPool, device);
}

VkImageView Util::createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect, uint32_t mipLevels)
{
	VkImageViewCreateInfo imgCreateInfo = {};
	VkImageView ret = VK_NULL_HANDLE;

	imgCreateInfo.subresourceRange.aspectMask = aspect;
	imgCreateInfo.subresourceRange.baseMipLevel = 0;
	imgCreateInfo.subresourceRange.levelCount = mipLevels;
	imgCreateInfo.subresourceRange.baseArrayLayer = 0;
	imgCreateInfo.subresourceRange.layerCount = 1;
	imgCreateInfo.format = format;
	imgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imgCreateInfo.image = image;
	imgCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

	ErrorCheck(vkCreateImageView(device, &imgCreateInfo, nullptr, &ret));

	return ret;
}