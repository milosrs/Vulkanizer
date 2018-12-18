#pragma once
#include "Buffer.h"

class IndexBuffer : public Buffer {
public:
	IndexBuffer(VkDevice, VkPhysicalDeviceMemoryProperties, std::vector<uint16_t>);
	~IndexBuffer();
	void fillBuffer();

	std::vector<uint16_t> getIndices();
private:
	std::vector<uint16_t> indices;
};