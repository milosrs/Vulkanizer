#pragma once
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#endif // !MAIN_WINDOW_H

#include "PLATFORM.h"
#include "BUILD_OPTIONS.h"
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>
#include <vector>
#include <array>

#include "Swapchain.h"
#include "RenderPass.h"
#include "FrameBuffer.h"
#include "Pipeline.h"
#include "CommandBufferHandler.h"
#include "DepthTester.h"
#include "Texture.h"
#include "Multisample.h"
#include "Renderer.h"

class QueueFamilyIndices;
class Vertices;
class Buffer;
class IndexBuffer;
class UniformBuffer;
class DescriptorHandler;
class RenderObject;
class WindowController;
struct Vertex;

namespace vkglTF {
	struct Vertex;
};

class MainWindow
{
public:
	static MainWindow& getInstance()
	{
		static MainWindow instance;

		return instance;
	};

	~MainWindow();

	void continueInitialization();

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
	MainWindow() {
		setWindowData(800, 600, "Hello World!");
		initialize();
	};

	void initialize();
	void setWindowData(uint32_t sizeX, uint32_t sizeY, std::string windowName);
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
	std::unique_ptr<Renderer> renderer = nullptr;

	std::vector<RenderObject*> objects;

	GLFWwindow* window = nullptr;

	VkSurfaceKHR surfaceKHR = VK_NULL_HANDLE;
	VkSurfaceCapabilitiesKHR surfaceCapatibilities = {};
	VkBool32 isWSISupported = false;
	VkSurfaceFormatKHR surfaceFormat = {};
	VkPresentInfoKHR presentInfo{};

	uint32_t sizeX;
	uint32_t sizeY;
	std::string name;
};

