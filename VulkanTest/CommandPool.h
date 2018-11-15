#pragma once
#include "PLATFORM.h"
#include "BUILD_OPTIONS.h"
#include "Util.h"

class CommandPool
{
public:
	CommandPool();
	CommandPool(uint32_t graphicsFamilyIndex, VkDevice* device);
	~CommandPool();

	VkCommandPool getCommandPool();
private:
	VkCommandPool cmdPool;
	VkCommandPoolCreateInfo cmdPoolCreateInfo{};
	VkDevice device;

	Util* util;
};

