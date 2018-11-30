#include "pch.h"
#include "CommandPool.h"


CommandPool::CommandPool()
{
}

CommandPool::CommandPool(uint32_t graphicsFamilyIndex, VkDevice* device, VkCommandPoolCreateFlags createFlags)
{
	this->util = &Util::instance();
	this->device = *device;

	cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
	cmdPoolCreateInfo.flags = createFlags;

	vkCreateCommandPool(*device, &cmdPoolCreateInfo, nullptr, &cmdPool);
}

CommandPool::~CommandPool()
{
	vkDestroyCommandPool(this->device, this->cmdPool, nullptr);
	this->cmdPool = nullptr;
}

VkCommandPool CommandPool::getCommandPool() {
	return this->cmdPool;
}