#include "pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkDeviceSize size) : Buffer(device, memoryProperties, size)
{
	this->initBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
						this->size,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
}

IndexBuffer::~IndexBuffer()
{
}

void IndexBuffer::fillBuffer(std::vector<uint32_t> indices)
{
	void* data;
	vkMapMemory(device, bufferMemory, 0, size, 0, &data);
	memcpy(data, indices.data(), (size_t)size);
	vkUnmapMemory(device, bufferMemory);
}