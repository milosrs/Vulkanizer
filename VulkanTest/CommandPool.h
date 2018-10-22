#pragma once
#include "Renderer.h"
#include "Util.h"

class CommandPool
{
public:
	CommandPool();
	~CommandPool();

	VkCommandPool getCommandPool();
private:
	VkCommandPool cmdPool;
	VkCommandPoolCreateInfo cmdPoolCreateInfo{};
};

