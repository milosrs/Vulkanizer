#pragma once

#include <vector>
#include <assert.h>
#include <optional>
#include <set>
#include "PLATFORM.h"

class QueueFamilyIndices
{
public:
	QueueFamilyIndices();
	QueueFamilyIndices(VkPhysicalDevice*, VkSurfaceKHR*  = VK_NULL_HANDLE);
	~QueueFamilyIndices();

	void createQueueCreateInfos();
	void createQueues(VkDevice* device);

	uint32_t getGraphicsFamilyIndex();
	uint32_t getPresentationFamilyIndex();
	VkQueue getQueue();
	VkQueue* getQueuePTR();
	std::vector<VkQueue> getQueues();
	std::vector<VkDeviceQueueCreateInfo> getQueueCreateInfos();
private:
	VkBool32 isPresentationSupported = false;
	
	std::optional<uint32_t> graphicsFamilyIndex;
	std::optional<uint32_t> presentFamily;

	uint32_t familyCount = 0;

	std::vector<VkQueue> queues;						//[0] render, [1] presentation
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	bool isComplete() {
		return graphicsFamilyIndex.has_value() && presentFamily.has_value();
	}
};

