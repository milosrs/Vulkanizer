#include "pch.h"
#include "VertexBuffer.h"


VertexBuffer::VertexBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties properties, std::shared_ptr<Vertices> vertices) : Buffer(device, properties)
{
	this->vertices = std::make_shared<Vertices>();
	this->vertices->setIndices(vertices->getIndices());
	this->vertices->setVertices(vertices->getVertices());

	this->size = sizeof(vertices->getVertices()[0]) * vertices->getVertices().size();

	VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

	this->initBuffer(usage, size, memoryPropertyFlags);
}

VertexBuffer::~VertexBuffer()
{
}

void VertexBuffer::fillBuffer()
{
	void* data;
	vkMapMemory(device, bufferMemory, 0, size, 0, &data);
	memcpy(data, vertices->getVertices().data(), (size_t)size);
	vkUnmapMemory(device, bufferMemory);
}


std::vector<Vertex> VertexBuffer::getVertices()
{
	return vertices->getVertices();
}

std::vector<uint16_t> VertexBuffer::getIndices()
{
	return vertices->getIndices();
}
