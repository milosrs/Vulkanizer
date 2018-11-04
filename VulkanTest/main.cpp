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

void recordFrameBuffer(CommandBuffer cmdBuffer, MainWindow* window, EpilepsyWarning EPILEPSY_WARNING_DELETE_THIS_IF_SENSITIVE);

/*vkCmdPipelineBarrier - kreiramo barijeru nad nekim delovima pajplajna. Ako dodjemo do trazenog dela pajplajna, pajplajn se zaustavlja dok se posao ne zavrsi.*/
int main()
{
	Util& util = Util::instance();
	Renderer renderer = Renderer();
	MainWindow* window = renderer.createWindow(800, 600, "RikisWindow");

	renderer.continueInitialization();
	window->continueInitialization();
	
	CommandPool cmdPool = CommandPool();
	CommandBuffer cmdBuffer = CommandBuffer(cmdPool.getCommandPool());
	
	VkSemaphore renderSemaphore = nullptr;
	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	CommandBufferSemaphoreInfo semaphoreInfo = CommandBufferSemaphoreInfo(true, renderSemaphore, NULL);
	EpilepsyWarning EPILEPSY_WARNING_DELETE_THIS_IF_SENSITIVE = EpilepsyWarning();							//Ovo ce biti renderovano

	while (renderer.run()) {
		//Ispisi FPS
		util.printFPS();

		//Pocni renderovanje
		window->beginRender();

		//Snimimo cmdBuffer (Sve do cmdBuffer.endRecording() je snimanje sadrzaja bafera)
		recordFrameBuffer(cmdBuffer, window, EPILEPSY_WARNING_DELETE_THIS_IF_SENSITIVE);

		//Submitujemo command buffer
		cmdBuffer.submitQueue(renderer.getQueue(), NULL, &semaphoreInfo);

		vkCreateSemaphore(renderer.getDevice(), &semaphoreCreateInfo, nullptr, &renderSemaphore);
		//Zavrsi renderovanje, tj prezentuj sliku na povrsini
		window->endRender({ renderSemaphore });
	}
	
	vkQueueWaitIdle(renderer.getQueue());
	vkDestroySemaphore(renderer.getDevice(), renderSemaphore, nullptr);
	cmdPool.~CommandPool();

	return 0;
}

void recordFrameBuffer(CommandBuffer cmdBuffer, MainWindow* window, EpilepsyWarning EPILEPSY_WARNING_DELETE_THIS_IF_SENSITIVE) {
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