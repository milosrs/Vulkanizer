#include "pch.h"
#include "DepthTester.h"
#include "Util.h"

static int instanceCount = 0;

DepthTester::DepthTester(VkDevice device, VkPhysicalDevice gpu, VkPhysicalDeviceMemoryProperties memprops)
{
	this->device = device;
	this->gpu = gpu;
	this->memprops = memprops;
	this->depthFormat = findFormat(device, gpu);
	
	instanceCount++;
}


DepthTester::~DepthTester()
{
	vkDestroyImageView(device, depthView, nullptr);
	vkDestroyImage(device, depth, nullptr);
	vkFreeMemory(device, depthMemory, nullptr);
}

void DepthTester::createDepthImage(uint32_t width, uint32_t height, VkCommandPool cmdPool, VkQueue queue, 
	VkSampleCountFlagBits samples)
{
	Util::createImage(width, height, 1, this->depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&this->depth,
		&this->depthMemory,
		this->device, &this->memprops, samples);

	this->depthView = Util::createImageView(device, depth, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	Util::transitionImageLayout(depth, depthFormat,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		cmdPool, queue, device, 1);
}

VkImageView DepthTester::getDepthImageView()
{
	return depthView;
}

VkImage DepthTester::getDepthImage()
{
	return depth;
}

VkFormat DepthTester::findFormat(VkDevice device, VkPhysicalDevice gpu)
{
	return Util::findSupportedFormat(
		gpu, device,
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool DepthTester::isInstanceCreated()
{
	return instanceCount > 0;
}
