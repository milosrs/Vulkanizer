#pragma once
#ifndef BUFFER_H
#define BUFFER_H
#endif
#include "PLATFORM.h"

class Vertices;

class Buffer
{
public:
	Buffer();
	Buffer(VkDevice, VkPhysicalDeviceMemoryProperties, VkDeviceSize = 0);
	~Buffer();

	void initBuffer(VkBufferUsageFlags, VkDeviceSize, VkMemoryPropertyFlags);
	void setDeviceSize(VkDeviceSize size);

	VkBuffer getBuffer();
	VkBuffer* getBufferPTR();
	VkDeviceSize getSize();
	void* getData();

	void setBuffer(VkBuffer);
	void setSize(VkDeviceSize);
	void setData(void* data);
	void bindBufferData();
protected:
	void *data;
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkDeviceSize size = VK_NULL_HANDLE;
	VkMemoryRequirements memoryRequirements = {};
	VkDeviceMemory bufferMemory = VK_NULL_HANDLE;;
	VkPhysicalDeviceMemoryProperties deviceMemoryProps = {};

	VkMemoryAllocateInfo allocateInfo = {};
	VkBufferCreateInfo bufferInfo = {};
};

