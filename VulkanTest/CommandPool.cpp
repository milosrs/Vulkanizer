#include "pch.h"
#include "CommandPool.h"


CommandPool::CommandPool(uint32_t graphicsFamilyIndex, VkDevice* device)
{
	this->util = &Util::instance();
	this->device = *device;

	cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
	cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	//Informacije u poolu su kratkog zivota, i pool ce biti resetovan nekada.
	//cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

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