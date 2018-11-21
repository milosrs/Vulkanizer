#include "pch.h"
#include <iostream>
#include "vulkan\vulkan.h"
#include "Renderer.h"
#include "CommandPool.h"
#include "Util.h"
#include "CommandBuffer.h"
#include "Semaphore.h"
#include "MainWindow.h"
#include "ColorChanger.h"
#include "Triangle.h"
#include "Pipeline.h"
#include <array>
#include <chrono>

void renderTriangle(Util*, Renderer*, MainWindow*, CommandPool*, CommandBuffer*, Pipeline*);
void renderColors(Util*, Renderer*, MainWindow*, CommandPool*, CommandBuffer*, Pipeline*);
void recordFrameBuffer(ColorChanger, CommandBuffer*, MainWindow*, Pipeline*);
void renderModes(int, Util*, Renderer*, MainWindow*, CommandPool*, CommandBuffer*, Pipeline*);

int main(int argn, char** argv)
{
	Util* util = &Util::instance();
	std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>();
	MainWindow* window = nullptr;
	
	renderer->createWindow(800, 600, "RikisWindow");

	window = renderer->getMainWindowPTR();

	renderer->continueInitialization();
	window->continueInitialization(renderer.get());

	std::unique_ptr<CommandPool> cmdPool = std::make_unique<CommandPool>(renderer->getQueueIndices()->getGraphicsFamilyIndex(), renderer->getDevicePTR());
	std::unique_ptr<CommandBuffer> cmdBuffer = std::make_unique<CommandBuffer>(cmdPool->getCommandPool(), renderer->getDevice());

	float viewportWidth = window->getSurfaceCapatibilities().currentExtent.width;
	float viewportHeight = window->getSurfaceCapatibilities().currentExtent.height;
	VkExtent2D scissorsExtent = window->getSurfaceCapatibilities().currentExtent;

	std::unique_ptr<Pipeline> pipeline = std::make_unique<Pipeline>(renderer->getDevicePTR(), window->getRenderPass()->getRenderPassPTR(), viewportWidth, viewportHeight, scissorsExtent);

	renderModes(argn, util, renderer.get(), window, cmdPool.get(), cmdBuffer.get(), pipeline.get());

	return 0;
}

void renderModes(int argn, Util* util, Renderer* renderer, MainWindow* window, CommandPool* cmdPool, CommandBuffer* cmdBuffer, Pipeline* pipeline) {
	switch (argn) {
	case 1: renderColors(util, renderer, window, cmdPool, cmdBuffer, pipeline);
	case 2: renderTriangle(util, renderer, window, cmdPool, cmdBuffer, pipeline);
	}
}

void renderTriangle(Util* util, Renderer* renderer, MainWindow* window, CommandPool* cmdPool, CommandBuffer* cmdBuffer, Pipeline* pipeline) {
	
	while (!glfwWindowShouldClose(window->getWindowPTR())) {
		glfwPollEvents();
		Triangle triangle = Triangle(window, renderer, cmdBuffer, cmdPool);

		triangle.render(pipeline->getViewportPTR());
	}
}

void renderColors(Util* util, Renderer* renderer, MainWindow* window, CommandPool* cmdPool, CommandBuffer* cmdBuffer, Pipeline* pipeline) {
	ColorChanger colorChanger(window, renderer, cmdBuffer, cmdPool);

	colorChanger.render(pipeline->getViewportPTR());
	
	cmdPool->~CommandPool();
}

