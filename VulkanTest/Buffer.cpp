#include "pch.h"
#include "Buffer.h"


Buffer::Buffer(VkDevice device, VkPhysicalDeviceMemoryProperties deviceMemoryProps)
{
	this->device = device;
	this->deviceMemoryProps = deviceMemoryProps;
	this->util = &Util::instance();
}

Buffer::~Buffer()
{
	vkDestroyBuffer(device, buffer, nullptr);
	vkFreeMemory(device, bufferMemory, nullptr);
}

void Buffer::initBuffer(VkBufferUsageFlags usage, VkDeviceSize size, 
						VkMemoryPropertyFlags memPropFlags, VkBuffer& buffer, VkDeviceMemory deviceMemory)
{
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.usage = usage;
	bufferInfo.size = size;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	util->ErrorCheck(vkCreateBuffer(device, &bufferInfo, nullptr, &this->buffer));
	vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = util->findMemoryTypeIndex(&this->deviceMemoryProps, &memoryRequirements, memPropFlags);

	util->ErrorCheck(vkAllocateMemory(device, &allocateInfo, nullptr, &bufferMemory));
	util->ErrorCheck(vkBindBufferMemory(device, buffer, bufferMemory, 0));
}

VkBuffer Buffer::getBuffer()
{
	return this->buffer;
}

VkDeviceSize Buffer::getSize()
{
	return size;
}
