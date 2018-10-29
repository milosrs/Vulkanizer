#include "pch.h"
#include "CommandPool.h"


CommandPool::CommandPool()
{
	Util& util = Util::instance();
	VkDevice device = util.getDevice();
	uint32_t gfi = util.getGraphicsFamilyIndex();

	cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreateInfo.queueFamilyIndex = gfi;
	cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	//Informacije u poolu su kratkog zivota, i pool ce biti resetovan nekada.
	//cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	vkCreateCommandPool(device, &cmdPoolCreateInfo, nullptr, &cmdPool);
}


CommandPool::~CommandPool()
{
	Util& util = Util::instance();

	vkDestroyCommandPool(util.getDevice(), this->cmdPool, nullptr);
	this->cmdPool = nullptr;
}

VkCommandPool CommandPool::getCommandPool() {
	return this->cmdPool;
}