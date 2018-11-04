#include "pch.h"
#include "QueueFamilyIndices.h"


QueueFamilyIndices::QueueFamilyIndices(VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface)
{
	bool foundWantedQueue = false;
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &familyCount, nullptr);
	queueFamilyProperties.resize(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &familyCount, queueFamilyProperties.data());

	int i = 0;
	for (const auto& queueFamilyProp : queueFamilyProperties) {
		if (surface != VK_NULL_HANDLE) {
			vkGetPhysicalDeviceSurfaceSupportKHR(*physicalDevice, i, *surface, &this->isPresentationSupported);
		}
		if (queueFamilyProp.queueCount > 0 && isPresentationSupported && queueFamilyProp.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			this->graphicsFamilyIndex = i;
			foundWantedQueue = true;
			break;
		}

		++i;
	}

	if (!foundWantedQueue) {
		assert(0 && "Vulkan Error: No GRAPHICS queue family found.");
		exit(-1);
	}
}


QueueFamilyIndices::~QueueFamilyIndices()
{
}

uint32_t QueueFamilyIndices::getGraphicsFamilyIndex()
{
	uint32_t ret = NULL;

	if (this->isComplete()) {
		ret = graphicsFamilyIndex.value();
	}

	return ret;
}

VkQueue QueueFamilyIndices::getQueue()
{
	return this->queues[0];
}

VkQueue* QueueFamilyIndices::getQueuePTR()
{
	return &this->queues[0];
}

std::vector<VkQueue> QueueFamilyIndices::getQueues()
{
	return this->queues;
}

VkDeviceQueueCreateInfo QueueFamilyIndices::createQueue(VkDevice* device)
{
	if (this->isComplete()) {
		for (VkQueue& queue : queues) {
			VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
			float queuePriorities[]{ 1.0f };														//Ovo pomaze Vulkan Core-u da skonta koji red je najprioritetniji za zavrsavanje zadataka iz Command Buffera

			deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo.queueFamilyIndex = this->graphicsFamilyIndex.value();
			deviceQueueCreateInfo.queueCount = 1;
			deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

			vkGetDeviceQueue(*device, this->graphicsFamilyIndex.value(), 0, &queue);			//Iz kog reda hocemo da fetchujemo? Mozemo imati vise queue...
		}
	}
	else {
		assert(0 && "Vulkan Error: No queue family avaiable for this device. Aborting.");
		exit(-1);
	}
}
