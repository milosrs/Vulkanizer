#pragma once
#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H
#endif
#include "Buffer.h"

class IndexBuffer : public Buffer {
public:
	IndexBuffer(VkDevice, VkPhysicalDeviceMemoryProperties, VkDeviceSize = 0);
	~IndexBuffer();
	void fillBuffer(std::vector<uint32_t> indices);
private:
};