#pragma once
#include "PLATFORM.h"
#include "Util.h"

class CommandBufferSemaphoreInfo
{
public:
	CommandBufferSemaphoreInfo(bool, VkSemaphore, VkPipelineStageFlags*);
	~CommandBufferSemaphoreInfo();

	bool getShouldWaitForSignalization();
	VkSemaphore getSemaphore();
	VkSemaphore* getSemaphorePTR();
	VkPipelineStageFlags* getPipelineStages();

	void setShouldWaitForSignalization(bool);
	void setSemaphore(VkSemaphore);
	void setPipelineStages(VkPipelineStageFlags*);
private:
	bool shouldWaitForSignalization;
	VkSemaphore semaphore = VK_NULL_HANDLE;
	VkPipelineStageFlags* pipelineStages = nullptr;
	
	Util* util;
};

