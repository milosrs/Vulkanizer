#pragma once
#ifndef COMMAND_BUFFER_HANDLER_H
#define COMMAND_BUFFER_HANDLER_H
#endif
#include "PLATFORM.h"
#include <vector>
#include <map>

class UniformBuffer;
class CommandBufferSemaphoreInfo;
class DescriptorHandler;
class MainWindow;
class RenderObject;
class IndexBuffer;

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
	CommandBufferHandler(uint32_t, VkDevice);
	~CommandBufferHandler();

	/*
	  1. Command buffer count,
	  2. Clear values for them
	*/
	void createDrawingCommandBuffers(uint32_t, RenderObject*);

	/*	
		cmdBufferIndex - Index command buffera za submit
		VkQueue - U koji red bi trebalo da se submituje posao bafera.
		VkPipelineStageFlags - U kom trenutku u pipeline-u Vulkan Core-a bi semafori trebalo da reaguju na ovaj submit
		ComandBufferSemaphoreInfo - koji semafor da cekamo
		ComandBufferSemaphoreInfo  - koji semafor da signaliziramo
		VkFence - ograda za submit
	*/
	bool submitQueue(int, VkQueue, CommandBufferSemaphoreInfo* = nullptr, CommandBufferSemaphoreInfo* = nullptr, VkFence* = nullptr);

	static VkCommandBuffer createOneTimeUsageBuffer(VkCommandPool, VkDevice);
	static void endOneTimeUsageBuffer(VkCommandBuffer, VkQueue, VkCommandPool, VkDevice);
	static void copyBuffer(VkBuffer, VkBuffer, VkDeviceSize, VkQueue, VkCommandPool, VkDevice);

	std::vector<CommandBuffer> getCommandBuffers();
	VkCommandPool getCommandPool();
private:
	VkCommandPool cmdPool = VK_NULL_HANDLE;
	VkCommandPool transferCommandPool = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;

	VkRenderPassBeginInfo renderPassBeginInfo{};
	VkRect2D renderArea{};

	VkFenceCreateInfo fenceCreateInfo = {};
	std::vector<CommandBuffer> commandBuffers;

	uint32_t drawingBuffersCount = 0;
	MainWindow* window = nullptr;
};

