#include "pch.h"
#include "Buffer.h"

Buffer::Buffer(VkDevice device, VkPhysicalDeviceMemoryProperties deviceMemoryProps, VkDeviceSize size)
{
	this->size = size;
	this->device = device;
	this->deviceMemoryProps = deviceMemoryProps;
}

Buffer::~Buffer()
{
	vkDestroyBuffer(device, buffer, nullptr);
	vkFreeMemory(device, bufferMemory, nullptr);
}

void Buffer::initBuffer(VkBufferUsageFlags usage, VkDeviceSize size, 
						VkMemoryPropertyFlags memPropFlags)
{
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.usage = usage;
	bufferInfo.size = size;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	Util::ErrorCheck(vkCreateBuffer(device, &bufferInfo, nullptr, &this->buffer));
	vkGetBufferMemoryRequirements(device, this->buffer, &memoryRequirements);

	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = Util::findMemoryTypeIndex(&this->deviceMemoryProps, &memoryRequirements, memPropFlags);

	Util::ErrorCheck(vkAllocateMemory(device, &allocateInfo, nullptr, &bufferMemory));
	Util::ErrorCheck(vkBindBufferMemory(device, this->buffer, bufferMemory, 0));
}

void Buffer::setDeviceSize(VkDeviceSize size)
{
	this->size = size;
}

VkBuffer Buffer::getBuffer()
{
	return this->buffer;
}

VkBuffer * Buffer::getBufferPTR()
{
	return &this->buffer;
}

VkDeviceSize Buffer::getSize()
{
	return size;
}
