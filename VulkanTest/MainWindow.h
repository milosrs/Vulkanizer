#pragma once
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#endif // !MAIN_WINDOW_H

#include "PLATFORM.h"
#include "BUILD_OPTIONS.h"
#include "Util.h"
#include "Util.h"
#include "Swapchain.h"
#include "RenderPass.h"
#include "FrameBuffer.h"
#include "Pipeline.h"
#include "QueueFamilyIndices.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "Vertices.h"
#include "Buffer.h"
#include "VertexBuffer.h"
#include "StagingBuffer.h"
#include "IndexBuffer.h"
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>
#include <vector>
#include <array>

class Renderer;

class MainWindow
{
public:
	MainWindow(Renderer* renderer, uint32_t sizeX, uint32_t sizeY, std::string windowName);
	MainWindow(const MainWindow&);
	~MainWindow();

	void continueInitialization(Renderer*);

	/*Koji semafor cekamo da signalizira dobavljenu sliku, viewport koji treba postaviti. NULL ako ne treba viewport.*/
	void beginRender(VkSemaphore);

	void endRender(std::vector<VkSemaphore>);
	void recreateSwapchain();

	/*CommandBuffer: Recording command buffer
	  bool: Is this drawing using index buffer*/
	void draw(VkCommandBuffer, bool);
	void setupPipeline(std::shared_ptr<Vertices>);
	void bindPipeline(VkCommandBuffer);

	Renderer* getRenderer();
	RenderPass* getRenderPass();
	FrameBuffer* getActiveFrameBuffer();
	Swapchain* getSwapchain();
	GLFWwindow* getWindowPTR();
	Pipeline* getPipelinePTR();
	std::vector< CommandBuffer*> getCommandBuffers();
	CommandPool* getCommandPoolPTR();

	VkSurfaceKHR getSurface();
	VkSurfaceKHR* getSurfacePTR();
	VkExtent2D getSurfaceSize();
	VkSurfaceCapabilitiesKHR getSurfaceCapatibilities();
	VkBool32 getIsWSISupported();
	VkSurfaceFormatKHR getSurfaceFormat();
	bool windowResized = true;
private:
	void InitOSWindow();
	void DeinitOSWindow();
	void InitOSSurface();

	void InitSurface();
	void DestroySurface();

	void choosePreferedFormat();
	void destroySwapchainDependencies();
	void createData();

	std::unique_ptr<Swapchain> swapchain = nullptr;
	std::unique_ptr<RenderPass> renderPass = nullptr;
	std::unique_ptr<FrameBuffer> frameBuffer = nullptr;
	std::unique_ptr<Pipeline> pipeline = nullptr;
	std::unique_ptr<CommandPool> cmdPool = nullptr;
	std::unique_ptr<CommandPool> transferCommandPool = nullptr;

	std::unique_ptr<IndexBuffer> indexBuffer = nullptr;
	std::unique_ptr<VertexBuffer> vertexBuffer = nullptr;
	
	std::vector<CommandBuffer*> cmdBuffers;

	Renderer* renderer = nullptr;
	GLFWwindow* window = nullptr;
	Util* util = nullptr;

	VkSurfaceKHR surfaceKHR = VK_NULL_HANDLE;
	VkSurfaceCapabilitiesKHR surfaceCapatibilities = {};
	VkBool32 isWSISupported = false;
	VkSurfaceFormatKHR surfaceFormat = {};
	VkPresentInfoKHR presentInfo{};

	uint32_t sizeX;
	uint32_t sizeY;
	std::string name = "MainWindow";
};

