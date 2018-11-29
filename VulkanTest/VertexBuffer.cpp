#include "pch.h"
#include "VertexBuffer.h"


VertexBuffer::VertexBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties deviceMemoryProps)
{
	this->device = device;
	this->deviceMemoryProps = deviceMemoryProps;
	this->util = &Util::instance();
}

VertexBuffer::~VertexBuffer()
{
	vkDestroyBuffer(device, buffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);
}

void VertexBuffer::initVertexBuffer(std::shared_ptr<Vertices> vertices)
{
	this->vertices = std::make_shared<Vertices>(*vertices);

	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.size = sizeof(vertices->getVertices()[0]) * vertices->getVertices().size();
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	util->ErrorCheck(vkCreateBuffer(device, &bufferInfo, nullptr, &this->buffer));
	vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = util->findMemoryTypeIndex(&this->deviceMemoryProps, &memoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	util->ErrorCheck(vkAllocateMemory(device, &allocateInfo, nullptr, &vertexBufferMemory));
	util->ErrorCheck(vkBindBufferMemory(device, buffer, vertexBufferMemory, 0));
}

void VertexBuffer::fillBuffer()
{
	void* data;
	vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, vertices->getVertices().data(), (size_t)bufferInfo.size);
	vkUnmapMemory(device, vertexBufferMemory);
}

VkBuffer VertexBuffer::getBuffer()
{
	return this->buffer;
}

std::vector<Vertex> VertexBuffer::getVertices()
{
	return vertices->getVertices();
}
