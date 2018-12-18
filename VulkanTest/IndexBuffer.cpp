#include "pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, std::vector<uint16_t> indices) : Buffer(device, memoryProperties)
{
	this->indices = indices;
	this->size = sizeof(indices[0]) * indices.size();
	this->initBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
						this->size,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
}

IndexBuffer::~IndexBuffer()
{
}

void IndexBuffer::fillBuffer()
{
	void* data;
	vkMapMemory(device, bufferMemory, 0, size, 0, &data);
	memcpy(data, indices.data(), (size_t)size);
	vkUnmapMemory(device, bufferMemory);
}

std::vector<uint16_t> IndexBuffer::getIndices()
{
	return indices;
}
