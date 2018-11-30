#include "pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, std::vector<uint16_t> indices) : Buffer(device, memoryProperties)
{
	this->indices = indices;
	this->size = sizeof(indices[0]) * indices.size();
	this->initBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
						this->size,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						buffer,
						bufferMemory);
}

IndexBuffer::~IndexBuffer()
{
}

std::vector<uint16_t> IndexBuffer::getIndices()
{
	return indices;
}
