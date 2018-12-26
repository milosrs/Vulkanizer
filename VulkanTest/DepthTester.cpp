#include "pch.h"
#include "DepthTester.h"

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
}

void DepthTester::createDepthImage(uint32_t width, uint32_t height, VkCommandPool cmdPool, VkQueue queue)
{
	Util::createImage(width, height, this->depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&this->depth,
		&this->depthMemory,
		this->device, &this->memprops);

	this->depthView = Util::createImageView(device, depth, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	Util::transitionImageLayout(&depth, depthFormat,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		cmdPool, queue, device);
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
