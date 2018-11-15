#include "pch.h"
#include <iostream>
#include "vulkan\vulkan.h"
#include "Renderer.h"
#include "CommandPool.h"
#include "Util.h"
#include "CommandBuffer.h"
#include "Semaphore.h"
#include "MainWindow.h"
#include "EpilepsyWarning.h"
#include <array>
#include <chrono>

void renderTriangle();
void renderEpilepsy();
void recordFrameBuffer(EpilepsyWarning EPILEPSY_WARNING_DELETE_THIS_IF_SENSITIVE);
void renderModes(int argn);

static Util* util;
static Renderer renderer;
static MainWindow* window;
static CommandBuffer cmdBuffer;
static CommandPool cmdPool;

int main(int argn, char** argv)
{
	util = &Util::instance();
	renderer = Renderer();
	window = renderer.createWindow(800, 600, "RikisWindow");

	renderer.continueInitialization();
	window->continueInitialization(&renderer);

	cmdPool = CommandPool(renderer.getQueueIndices()->getGraphicsFamilyIndex(), renderer.getDevicePTR());
	cmdBuffer = CommandBuffer(cmdPool.getCommandPool(), renderer.getDevice());

	renderModes(argn);

	return 0;
}

void renderModes(int argn) {
	switch (argn) {
	case 0: renderEpilepsy();
	case 1: renderTriangle();
	}
}

void renderTriangle() {

}

void renderEpilepsy() {
	VkSemaphore renderSemaphore = nullptr;
	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	CommandBufferSemaphoreInfo semaphoreInfo = CommandBufferSemaphoreInfo(true, renderSemaphore, NULL);
	EpilepsyWarning EPILEPSY_WARNING_DELETE_THIS_IF_SENSITIVE = EpilepsyWarning();							//Ovo ce biti renderovano

	while (renderer.run()) {
		//Ispisi FPS
		util->printFPS();

		//Pocni renderovanje
		window->beginRender();

		//Snimimo cmdBuffer (Sve do cmdBuffer.endRecording() je snimanje sadrzaja bafera)
		recordFrameBuffer(EPILEPSY_WARNING_DELETE_THIS_IF_SENSITIVE);

		//Submitujemo command buffer
		cmdBuffer.submitQueue(renderer.getQueue(), NULL, &semaphoreInfo);

		vkCreateSemaphore(renderer.getDevice(), &semaphoreCreateInfo, nullptr, &renderSemaphore);
		//Zavrsi renderovanje, tj prezentuj sliku na povrsini
		window->endRender({ renderSemaphore });
	}

	vkQueueWaitIdle(renderer.getQueue());
	vkDestroySemaphore(renderer.getDevice(), renderSemaphore, nullptr);
	cmdPool.~CommandPool();
}

void recordFrameBuffer(EpilepsyWarning EPILEPSY_WARNING_DELETE_THIS_IF_SENSITIVE) {
	cmdBuffer.startRecording();

	VkRect2D renderArea{};
	renderArea.offset.x = 0;
	renderArea.offset.y = 0;
	renderArea.extent = window->getSurfaceSize();

	std::array<VkClearValue, 2> clearValues = EPILEPSY_WARNING_DELETE_THIS_IF_SENSITIVE.getNewClearValues();

	//Napravimo renderPassBeginInfo (ovo u klasu staviti)
	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.renderPass = window->getRenderPass().getRenderPass();
	renderPassBeginInfo.framebuffer = window->getActiveFrameBuffer().getActiveFrameBuffer(window->getSwapchain().getActiveImageSwapchain());
	renderPassBeginInfo.renderArea = renderArea;
	renderPassBeginInfo.clearValueCount = clearValues.size();
	renderPassBeginInfo.pClearValues = clearValues.data();

	//Zapocni render pass
	vkCmdBeginRenderPass(cmdBuffer.getCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdEndRenderPass(cmdBuffer.getCommandBuffer());

	cmdBuffer.endRecording();
}