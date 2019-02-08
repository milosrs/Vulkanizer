#include "pch.h"
#include "QueueFamilyIndices.h"

QueueFamilyIndices::QueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	if (physicalDevice != VK_NULL_HANDLE && surface != VK_NULL_HANDLE) {
		bool foundWantedQueue = false;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);
		queueFamilyProperties.resize(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, queueFamilyProperties.data());

		int i = 0;
		for (const auto& queueFamilyProp : queueFamilyProperties) {
			if (surface != VK_NULL_HANDLE) {
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &isPresentationSupported);

				if (queueFamilyProp.queueCount > 0 && isPresentationSupported) {
					this->presentFamily = i;
					foundWantedQueue = true;
				}
				if (queueFamilyProp.queueCount > 0 && queueFamilyProp.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					this->graphicsFamilyIndex = i;
					foundWantedQueue = true;
				}
			}
			else if (queueFamilyProp.queueCount > 0 && queueFamilyProp.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				this->graphicsFamilyIndex = i;
				foundWantedQueue = true;
			}

			++i;
		}

		if (!foundWantedQueue) {
			assert(0 && "Vulkan Error: No GRAPHICS queue family found.");
			exit(-1);
		}
	}
}


QueueFamilyIndices::~QueueFamilyIndices()
{
}

uint32_t QueueFamilyIndices::getGraphicsFamilyIndex()
{
	uint32_t ret = NULL;

	if (this->graphicsFamilyIndex.has_value()) {
		ret = graphicsFamilyIndex.value();
	}

	return ret;
}

uint32_t QueueFamilyIndices::getPresentationFamilyIndex()
{
	uint32_t ret = NULL;

	if (this->presentFamily.has_value()) {
		ret = presentFamily.value();
	}

	return ret;
}

uint32_t QueueFamilyIndices::getTransferFamilyIndex()
{
	uint32_t ret = NULL;

	if (this->transferFamily.has_value()) {
		ret = transferFamily.value();
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

std::vector<VkDeviceQueueCreateInfo> QueueFamilyIndices::getQueueCreateInfos() {
	return this->queueCreateInfos;
}

void QueueFamilyIndices::createQueueCreateInfos()
{
	std::set<uint32_t> uniqueQueueFamilies = { this->graphicsFamilyIndex.value(), this->presentFamily.value() };
	
	if (this->isComplete()) {
		for (uint32_t family : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
			float queuePriorities[]{ 1.0f };														//Ovo pomaze Vulkan Core-u da skonta koji red je najprioritetniji za zavrsavanje zadataka iz Command Buffera

			deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo.queueFamilyIndex = family;
			deviceQueueCreateInfo.queueCount = 1;
			deviceQueueCreateInfo.pQueuePriorities = queuePriorities;
			queueCreateInfos.push_back(deviceQueueCreateInfo);
		}
	}
	else {
		assert(0 && "Vulkan Error: No queue family avaiable for this device. Aborting.");
		exit(-1);
	}
}

void QueueFamilyIndices::createQueues(VkDevice device) {
	std::set<uint32_t> uniqueQueueFamilies = { this->graphicsFamilyIndex.value(), this->presentFamily.value() };

	for (uint32_t i = 0; i < uniqueQueueFamilies.size(); i++) {
		VkQueue queue = VK_NULL_HANDLE;
		vkGetDeviceQueue(device, this->graphicsFamilyIndex.value(), 0, &queue);			//Iz koje familije hocemo da uzmemo red

		queues.push_back(queue);
	}
}