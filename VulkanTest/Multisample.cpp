#include "pch.h"
#include "Multisample.h"
#include "Util.h"

Multisample::Multisample(VkDevice device, VkPhysicalDeviceMemoryProperties *memprops, VkCommandPool cmdPool, VkQueue queue,
	VkFormat format, uint32_t w, uint32_t h, VkSampleCountFlagBits samples)
{
	this->device = device;
	swapchainFormat = format;
	VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;


	Util::createImage(w, h, 1, swapchainFormat, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&image, &imageMemory, device, memprops, samples);

	imageView = Util::createImageView(device, image, swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	Util::transitionImageLayout(image, swapchainFormat, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, cmdPool, queue, device, 1);
}


Multisample::~Multisample()
{
	vkDestroyImageView(device, imageView, nullptr);
	vkDestroyImage(device,image, nullptr);
	vkFreeMemory(device, imageMemory, nullptr);
}

VkImageView Multisample::getView()
{
	return imageView;
}
