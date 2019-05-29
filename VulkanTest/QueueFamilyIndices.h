#pragma once
#ifndef QUEUE_FAMILY_INDICES_H
#define QUEUE_FAMILY_INDICES_H
#endif
#include <vector>
#include <assert.h>
#include <optional>
#include <set>
#include <map>
#include <tuple>
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
	
	std::map<QueueFamily, std::tuple<uint32_t, VkQueue>> queueFamilyMap;
	std::optional<uint32_t> graphicsFamilyIndex;
	std::optional<uint32_t> presentFamilyIndex;
	std::optional<uint32_t> transferFamilyIndex;
	std::optional<uint32_t> computeFamilyIndex;

	uint32_t familyCount = 0;

	std::vector<VkQueue> queues;						//[0] render, [1] presentation, [2] transfer, [3] compute
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	bool isComplete(std::optional<uint32_t> index) {
		return index.has_value();
	}

	std::set<uint32_t> getUniqueQueueFamilyIndices() {
		return std::set<uint32_t> { this->graphicsFamilyIndex.value(), this->presentFamilyIndex.value(), this->transferFamilyIndex.value() };
	}
};

