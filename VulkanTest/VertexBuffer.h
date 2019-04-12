#pragma once
#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H
#endif
#include "Buffer.h"
#include "PLATFORM.h"

template<typename T>
class VertexBuffer : public Buffer
{
public:
	VertexBuffer(VkDevice, VkPhysicalDeviceMemoryProperties, VkDeviceSize);
	~VertexBuffer();

	void fillBuffer(std::vector<T> vertices);
private:
};

