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
					this->presentFamilyIndex = i;
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

			else if (queueFamilyProp.queueCount > 0 &&
				(queueFamilyProp.queueFlags & VK_QUEUE_TRANSFER_BIT && !(queueFamilyProp.queueFlags & VK_QUEUE_GRAPHICS_BIT))
				) {
				this->transferFamilyIndex = i;
			}

			else if (queueFamilyProp.queueCount > 0 &&
				(queueFamilyProp.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
				this->computeFamilyIndex = i;
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

	if (this->presentFamilyIndex.has_value()) {
		ret = presentFamilyIndex.value();
	}

	return ret;
}

uint32_t QueueFamilyIndices::getTransferFamilyIndex()
{
	uint32_t ret = NULL;

	if (this->transferFamilyIndex.has_value()) {
		ret = transferFamilyIndex.value();
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
	std::set<uint32_t> uniqueQueueFamilies = getUniqueQueueFamilyIndices();

	if (this->isComplete(graphicsFamilyIndex) && this->isComplete(presentFamilyIndex)) {
		for (uint32_t family : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
			float queuePriorities[]{ 1.0f, 1.0f, 0.9f, 0.5f };														//Ovo pomaze Vulkan Core-u da skonta koji red je najprioritetniji za zavrsavanje zadataka iz Command Buffera

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
	std::set<uint32_t> uniqueQueueFamilies = getUniqueQueueFamilyIndices();

	for (uint32_t index : uniqueQueueFamilies) {
		VkQueue queue = VK_NULL_HANDLE;
		vkGetDeviceQueue(device, index, 0, &queue);

		queues.push_back(queue);
	}
}