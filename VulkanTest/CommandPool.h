#pragma once
#include "PLATFORM.h"
#include "BUILD_OPTIONS.h"
#include "Util.h"

class CommandPool
{
public:
	CommandPool();
	CommandPool(uint32_t graphicsFamilyIndex, VkDevice* device, VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
	~CommandPool();

	VkCommandPool getCommandPool();
private:
	VkCommandPool cmdPool = VK_NULL_HANDLE;
	VkCommandPoolCreateInfo cmdPoolCreateInfo{};
	VkDevice device = VK_NULL_HANDLE;;

	Util* util;
};

