#pragma once
#include <vulkan/vulkan.h>
class CommandBufferSemaphoreInfo
{
public:
	CommandBufferSemaphoreInfo();
	CommandBufferSemaphoreInfo(bool, VkSemaphore, VkPipelineStageFlags*);
	~CommandBufferSemaphoreInfo();

	bool getShouldWaitForSignalization();
	VkSemaphore getSemaphore();
	VkPipelineStageFlags* getPipelineStages();

	void setShouldWaitForSignalization(bool);
	void setSemaphore(VkSemaphore);
	void setPipelineStages(VkPipelineStageFlags*);
private:
	bool shouldWaitForSignalization;
	VkSemaphore semaphore;
	VkPipelineStageFlags* pipelineStages;
};

