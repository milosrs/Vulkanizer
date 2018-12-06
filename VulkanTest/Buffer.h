#pragma once
#include "PLATFORM.h"
#include "Vertices.h"
#include "Util.h"

class Buffer
{
public:
	Buffer(VkDevice, VkPhysicalDeviceMemoryProperties);
	~Buffer();

	void initBuffer(VkBufferUsageFlags, VkDeviceSize, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory);

	VkBuffer getBuffer();
	VkBuffer* getBufferPTR();
	VkDeviceSize getSize();
protected:
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkDeviceSize size = VK_NULL_HANDLE;
	VkMemoryRequirements memoryRequirements = {};
	VkDeviceMemory bufferMemory = VK_NULL_HANDLE;;
	VkPhysicalDeviceMemoryProperties deviceMemoryProps = {};

	VkMemoryAllocateInfo allocateInfo{};
	VkBufferCreateInfo bufferInfo{};

	Util* util = nullptr;
};

