#pragma once
#ifndef QUEUE_FAMILY_INDICES_H
#define QUEUE_FAMILY_INDICES_H
#endif
#include <vector>
#include <assert.h>
#include <optional>
#include <set>
#include "PLATFORM.h"

class QueueFamilyIndices
{
public:
	QueueFamilyIndices(VkPhysicalDevice, VkSurfaceKHR = VK_NULL_HANDLE);
	~QueueFamilyIndices();

	void createQueueCreateInfos();
	void createQueues(VkDevice device);

	uint32_t getGraphicsFamilyIndex();
	uint32_t getPresentationFamilyIndex();
	uint32_t getTransferFamilyIndex();

	VkQueue getQueue();
	VkQueue* getQueuePTR();
	std::vector<VkQueue> getQueues();
	std::vector<VkDeviceQueueCreateInfo> getQueueCreateInfos();
private:
	VkBool32 isPresentationSupported = false;
	
	std::optional<uint32_t> graphicsFamilyIndex;
	std::optional<uint32_t> presentFamily;
	std::optional<uint32_t> transferFamily;

	uint32_t familyCount = 0;

	std::vector<VkQueue> queues;						//[0] render, [1] presentation, [2] transfer
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	bool isComplete() {
		return graphicsFamilyIndex.has_value() && presentFamily.has_value();
	}
};

