#pragma once
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#endif // !MAIN_WINDOW_H

#include "PLATFORM.h"
#include "BUILD_OPTIONS.h"
#include "Swapchain.h"
#include "RenderPass.h"
#include "FrameBuffer.h"
#include "Pipeline.h"
#include "QueueFamilyIndices.h"
#include "Vertices.h"
#include "Buffer.h"
#include "VertexBuffer.h"
#include "StagingBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "DescriptorHandler.h"
#include "DepthTester.h"
#include "Texture.h"
#include "Multisample.h"
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>
#include <vector>
#include <array>

class Renderer;
class CommandBufferHandler;
class RenderObject;
class WindowController;

class MainWindow
{
public:
	MainWindow(Renderer* renderer, uint32_t sizeX, uint32_t sizeY, std::string windowName);
	MainWindow(const MainWindow&);
	~MainWindow();

	void continueInitialization(Renderer*);

	/*Koji semafor cekamo da signalizira dobavljenu sliku*/
	void beginRender(VkSemaphore);

	void endRender(std::vector<VkSemaphore>);
	void recreateSwapchain();

	/*Vertices: collection of vertices and indices
	  Uniform: is this pipeline using uniform buffers*/
	void setupPipeline(RenderObject *, bool);
	void bindPipeline(RenderObject*, VkCommandBuffer);

	Renderer* getRenderer();
	RenderPass* getRenderPass();
	FrameBuffer* getActiveFrameBuffer();
	Swapchain* getSwapchain();
	GLFWwindow* getWindowPTR();
	Pipeline* getPipelinePTR();
	CommandBufferHandler* getCommandHandler();
	std::vector<RenderObject*> getRenderObjects();

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
	std::unique_ptr<CommandBufferHandler> commandBufferHandler = nullptr;
	std::unique_ptr<Texture> background = nullptr;
	std::unique_ptr<DepthTester> depthTester = nullptr;
	std::unique_ptr<Multisample> multisampler = nullptr;
	std::unique_ptr<WindowController> windowController = nullptr;

	std::vector<RenderObject*> objects;

	Renderer* renderer = nullptr;
	GLFWwindow* window = nullptr;

	VkSurfaceKHR surfaceKHR = VK_NULL_HANDLE;
	VkSurfaceCapabilitiesKHR surfaceCapatibilities = {};
	VkBool32 isWSISupported = false;
	VkSurfaceFormatKHR surfaceFormat = {};
	VkPresentInfoKHR presentInfo{};

	uint32_t sizeX;
	uint32_t sizeY;
	std::string name = "MainWindow";
};

