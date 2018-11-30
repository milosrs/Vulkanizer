#pragma once
#include <vector>
#include "Util.h"
#include "CommandBufferSemaphoreInfo.h"

enum CommandBufferType {
	GRAPHICS = 0,
	TRANSFER = 1
};

/*Command Buffer pocinje izvrsavanje u redu u kom su submitovani. Komande unutar bafera se izvrsavaju u redu u kom su submitovane.*/
/*Ovo ne znaci da je posao npr prvog bafera gotov i tek onda drugi bafer krece sa radom.*/
/*Resenje ovoga: Semaphore.h*/
class CommandBuffer
{
public:
	CommandBuffer(VkCommandPool, VkDevice, VkCommandBufferUsageFlags, CommandBufferType);
	CommandBuffer(const CommandBuffer&);
	~CommandBuffer();

	void startCommandBuffer(VkViewport* viewport = nullptr);
	void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size, VkQueue queue);

	/*VkQueue - U koji red bi trebalo da se submituje posao bafera.
	VkPipelineStageFlags - U kom trenutku u pipeline-u Vulkan Core-a bi semafori trebalo da reaguju na ovaj submit
	ComandBufferSemaphoreInfo - koji semafor da cekamo
	ComandBufferSemaphoreInfo  - koji semafor da signaliziramo
	VkFence - ograda za submit*/
	bool submitQueue(VkDevice, VkQueue, CommandBufferSemaphoreInfo* = nullptr, CommandBufferSemaphoreInfo* = nullptr, VkFence* = nullptr);

	void allocateCommandBuffer();
	void endCommandBuffer();

	VkCommandBuffer getCommandBuffer();
	CommandBufferType getType();
private:
	VkCommandPool commandPool = VK_NULL_HANDLE;
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	VkCommandBufferAllocateInfo allocateInfo = {};
	VkCommandBufferBeginInfo beginInfo = {};
	VkFenceCreateInfo fenceCreateInfo = {};
	VkDevice device = VK_NULL_HANDLE;

	CommandBufferType type;

	Util* util;
};

