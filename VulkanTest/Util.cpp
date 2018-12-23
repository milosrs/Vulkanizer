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

VkDevice Util::getDevice()
{
	return device;
}

#else
void Util::ErrorCheck(VkResult result) {};
#endif

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

void Util::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
						VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory,
						VkDevice device, VkPhysicalDeviceMemoryProperties *memprops, VkDeviceSize size, 
						unsigned char* pixels, StagingBuffer<unsigned char>* stagingBuffer) {

	VkImageCreateInfo info = {};
	VkMemoryRequirements imageMemoryRequirements = {};
	VkMemoryAllocateInfo allocateInfo = {};

	
	stagingBuffer->fillBuffer(pixels);

	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.imageType = VK_IMAGE_TYPE_2D;											//Kakav koordinatni sistem koristimo za texturu?
	info.extent = { width, height, 1 };											//W, H, Depth mora biti 1.
	info.mipLevels = 1;
	info.arrayLayers = 1;
	info.format = format;														//Format bi trebao da bude kao format bafera
	info.tiling = VK_IMAGE_TILING_OPTIMAL;										//Da bi smo pristupali slici iz sejdera. Linear tilingom bi mogli pristupati texelima kako hocemo.
	info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;								//Preinitialized cuva podatke o texelima posle tranzicije.
	info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;	//Sampled bitom mozemo pristupiti texturi iz sejdera.
	info.samples = VK_SAMPLE_COUNT_1_BIT;										//Multisampling
	info.flags = 0;

	ErrorCheck(vkCreateImage(device, &info, nullptr, image));
	vkGetImageMemoryRequirements(device, *image, &imageMemoryRequirements);

	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.memoryTypeIndex = findMemoryTypeIndex(memprops, &imageMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	allocateInfo.allocationSize = imageMemoryRequirements.size;

	ErrorCheck(vkAllocateMemory(device, &allocateInfo, nullptr, imageMemory));
	ErrorCheck(vkBindImageMemory(device, *image, *imageMemory, 0));
}

void Util::transitionImageLayout(VkImage *image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, 
								VkCommandBuffer recordingBuffer)
{
	VkImageMemoryBarrier barrier = {};
	VkImageSubresourceRange subresourceRange = {};
	VkPipelineStageFlags sourceStage = {};
	VkPipelineStageFlags dstStage = {};

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("Layout transition not supported.");
	}

	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.layerCount = 1;
	subresourceRange.levelCount = 1;

	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = *image;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;

	/*Ova dva polja za redove moraju biti namestena da budu ignorisana, 
		ako necemo da prebacujemo vlasnistvo nad slikom iz jednog Queue-a u drugi.*/
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;	
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	vkCmdPipelineBarrier(recordingBuffer, sourceStage, dstStage, 0, 0, nullptr, 0, nullptr, 0, &barrier);
}

void Util::copyBufferToimage(VkCommandBuffer cmdBuffer, VkBuffer buffer, VkImage *image, VkImageLayout layout,
							uint32_t width, uint32_t height)
{
	VkBufferImageCopy region{};
	VkImageSubresourceLayers subresouce{};

	subresouce.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresouce.baseArrayLayer = 0;
	subresouce.mipLevel = 0;
	subresouce.layerCount = 1;

	region.bufferImageHeight = 0;
	region.bufferOffset = 0;									//Na kom mestu u baferu pocinju podaci o pikselima?
	region.bufferRowLength = 0;
	region.imageOffset = { 0, 0, 0 };							//Gde
	region.imageExtent = { width, height, 1 };					//Kopiramo
	region.imageSubresource = subresouce;						//Podatke o slici

	vkCmdCopyBufferToImage(cmdBuffer, buffer, *image, layout, 1, &region);
}

VkImageView Util::createImageView(VkDevice device, VkImage image, VkFormat format)
{
	VkImageViewCreateInfo imgCreateInfo = {};
	VkImageView ret = VK_NULL_HANDLE;

	imgCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imgCreateInfo.subresourceRange.baseMipLevel = 0;
	imgCreateInfo.subresourceRange.levelCount = 1;
	imgCreateInfo.subresourceRange.baseArrayLayer = 0;
	imgCreateInfo.subresourceRange.layerCount = 1;
	imgCreateInfo.format = format;
	imgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imgCreateInfo.image = image;
	imgCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

	ErrorCheck(vkCreateImageView(device, &imgCreateInfo, nullptr, &ret));

	return ret;
}