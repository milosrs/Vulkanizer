#pragma once
#ifndef COMMAND_BUFFER_HANDLER_H
#define COMMAND_BUFFER_HANDLER_H
#endif
#include <vector>
#include "Util.h"
#include "CommandBufferSemaphoreInfo.h"
#include <map>

class MainWindow;

enum CommandBufferType {
	GRAPHICS = 0,
	TRANSFER = 1
};

struct CommandBuffer {
	VkCommandBuffer commandBuffer;
	CommandBufferType type;
} typedef CommandBuffer;

class CommandBufferHandler
{
public:
	/* Graphics Family Index,
	   Device,
	   CommandPool Create Flags*/
	CommandBufferHandler(uint32_t, VkDevice, MainWindow* = nullptr);
	~CommandBufferHandler();

	/*How much of these?*/
	void createDrawingCommandBuffers(uint32_t);

	/*How much of these?*/
	void createTransferCommandBuffers(uint32_t);

	void copyBuffer(VkBuffer, VkBuffer, VkDeviceSize, VkQueue);

	/*cmdBufferIndex - Index command buffera za submit
	VkQueue - U koji red bi trebalo da se submituje posao bafera.
	VkPipelineStageFlags - U kom trenutku u pipeline-u Vulkan Core-a bi semafori trebalo da reaguju na ovaj submit
	ComandBufferSemaphoreInfo - koji semafor da cekamo
	ComandBufferSemaphoreInfo  - koji semafor da signaliziramo
	VkFence - ograda za submit*/
	bool submitQueue(int, VkQueue, CommandBufferSemaphoreInfo* = nullptr, CommandBufferSemaphoreInfo* = nullptr, VkFence* = nullptr);

	std::vector<CommandBuffer> getCommandBuffers();
	VkCommandPool getCommandPool();
private:
	VkCommandPool cmdPool = VK_NULL_HANDLE;
	VkCommandPool transferCommandPool = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;

	VkClearValue clearValues = { 0.0f, 0.0f, 0.0f, 1.0f };
	VkRenderPassBeginInfo renderPassBeginInfo{};
	VkRect2D renderArea{};

	VkFenceCreateInfo fenceCreateInfo = {};
	std::vector<CommandBuffer> commandBuffers;

	uint32_t drawingBuffersCount = 0;
	MainWindow* window = nullptr;
	Util* util;
};

