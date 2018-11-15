#pragma once
#include <vector>
#include "Util.h"
#include "CommandBufferSemaphoreInfo.h"

/*Command Buffer pocinje izvrsavanje u redu u kom su submitovani. Komande unutar bafera se izvrsavaju u redu u kom su submitovane.*/
/*Ovo ne znaci da je posao npr prvog bafera gotov i tek onda drugi bafer krece sa radom.*/
/*Resenje ovoga: Semaphore.h*/
class CommandBuffer
{
public:
	CommandBuffer();
	CommandBuffer(VkCommandPool, VkDevice);
	~CommandBuffer();

	void doSomeWork(VkQueue queue, VkViewport* viewport);
	void createFence();
	bool submitQueue(VkQueue, VkPipelineStageFlags*, CommandBufferSemaphoreInfo*);

	void startRecording();
	void endRecording();

	VkFence getFence();
	VkCommandBuffer getCommandBuffer();
private:
	VkCommandBuffer commandBuffer = nullptr;
	VkCommandBufferAllocateInfo allocateInfo = {};
	VkCommandBufferBeginInfo beginInfo = {};
	VkFence fence = nullptr;
	VkFenceCreateInfo fenceCreateInfo = {};
	VkDevice device;

	Util* util;
};

