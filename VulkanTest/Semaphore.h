#pragma once
#include <vulkan/vulkan.h>
#include "Util.h"

/*Semafor u VulkanGL govori VulkanCore kada odredjene funkcionalnosti mogu da se koriste*/
class Semaphore
{
public:
	Semaphore();
	~Semaphore();

	VkSemaphore getSemaphore();

private:
	VkSemaphore semaphore;
	VkSemaphoreCreateInfo semaphoreCreateInfo;
};

