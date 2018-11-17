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

void renderTriangle(Util*, Renderer*, MainWindow*, CommandPool*, CommandBuffer*);
void renderEpilepsy(Util*, Renderer*, MainWindow*, CommandPool*, CommandBuffer*);
void recordFrameBuffer(ColorChanger, CommandBuffer*, MainWindow*);
void renderModes(int, Util*, Renderer*, MainWindow*, CommandPool*, CommandBuffer*);

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
	std::unique_ptr<Pipeline> pipeline = std::make_unique<Pipeline>(renderer->getDevicePTR(), window->getRenderPass()->getRenderPass());

	renderModes(argn, util, renderer.get(), window, cmdPool.get(), cmdBuffer.get());

	return 0;
}

void renderModes(int argn, Util* util, Renderer* renderer, MainWindow* window, CommandPool* cmdPool, CommandBuffer* cmdBuffer) {
	switch (argn) {
	case 1: renderEpilepsy(util, renderer, window, cmdPool, cmdBuffer);
	case 2: renderTriangle(util, renderer, window, cmdPool, cmdBuffer);
	}
}

void renderTriangle(Util* util, Renderer* renderer, MainWindow* window, CommandPool* cmdPool, CommandBuffer* cmdBuffer) {
	
	while (!glfwWindowShouldClose(window->getWindowPTR())) {
		glfwPollEvents();
		Triangle triangle = Triangle(window, renderer, cmdBuffer, cmdPool);

		triangle.render();
	}
}

void renderEpilepsy(Util* util, Renderer* renderer, MainWindow* window, CommandPool* cmdPool, CommandBuffer* cmdBuffer) {
	ColorChanger colorChanger(window, renderer, cmdBuffer, cmdPool);

	colorChanger.render();
	
	cmdPool->~CommandPool();
}

