#include "pch.h"
#include "VertexBuffer.h"
#include "Vertices.h"
#include "glTFModel.hpp"

template<typename T>
VertexBuffer<T>::VertexBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties properties, VkDeviceSize size) : Buffer(device, properties, size)
{
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

	this->initBuffer(usage, size, memoryPropertyFlags);
}

template<typename T>
VertexBuffer<T>::~VertexBuffer()
{
}

template<typename T>
void VertexBuffer<T>::fillBuffer(std::vector<T> vertices)
{
	void* data;
	vkMapMemory(device, bufferMemory, 0, size, 0, &data);
	memcpy(data, vertices.data(), (size_t)size);
	vkUnmapMemory(device, bufferMemory);
}

template class VertexBuffer<Vertex>;
template class VertexBuffer<vkglTF::Vertex>;