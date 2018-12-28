#include "pch.h"
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties properties, VkDeviceSize size) : Buffer(device, properties, size)
{
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

	this->initBuffer(usage, size, memoryPropertyFlags);
}

VertexBuffer::~VertexBuffer()
{
}

void VertexBuffer::fillBuffer(std::vector<Vertex> vertices)
{
	void* data;
	vkMapMemory(device, bufferMemory, 0, size, 0, &data);
	memcpy(data, vertices.data(), (size_t)size);
	vkUnmapMemory(device, bufferMemory);
}