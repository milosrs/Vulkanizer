#include "pch.h"
#include "ColorChanger.h"

ColorChanger::ColorChanger(MainWindow* window, Renderer* renderer, CommandBuffer* cmdBuffer, CommandPool* cmdPool) : Scene(window, renderer, cmdBuffer, cmdPool)
{
	this->clearValues = std::array<VkClearValue, 2> {};
}

std::array<VkClearValue, 2> ColorChanger::getNewClearValues() {
	colorRotator += 0.000000001;

	clearValues[0].depthStencil.depth = 0.0f;
	clearValues[0].depthStencil.stencil = 0;
	clearValues[1].color.float32[0] = getColor(CIRCLE_THIRD_1);
	clearValues[1].color.float32[1] = getColor(CIRCLE_THIRD_2);
	clearValues[1].color.float32[2] = getColor(CIRCLE_THIRD_3);
	clearValues[1].color.float32[3] = 0.0f;

	return clearValues;
}

ColorChanger::~ColorChanger()
{
}

void ColorChanger::render() {
	VkSemaphore renderSemaphore = nullptr;
	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	CommandBufferSemaphoreInfo semaphoreInfo(true, renderSemaphore, NULL);

	while (renderer->run()) {
		//Ispisi FPS
		util->printFPS();

		//Pocni renderovanje
		window->beginRender();

		//Snimimo cmdBuffer (Sve do cmdBuffer.endRecording() je snimanje sadrzaja bafera)
		recordFrameBuffer(cmdBuffer, window);

		//Submitujemo command buffer
		cmdBuffer->submitQueue(renderer->getQueue(), NULL, &semaphoreInfo);

		vkCreateSemaphore(renderer->getDevice(), &semaphoreCreateInfo, nullptr, &renderSemaphore);
		//Zavrsi renderovanje, tj prezentuj sliku na povrsini
		window->endRender({ renderSemaphore });
	}

	vkQueueWaitIdle(renderer->getQueue());
	vkDestroySemaphore(renderer->getDevice(), renderSemaphore, nullptr);
}

void ColorChanger::recordFrameBuffer(CommandBuffer* cmdBuffer, MainWindow* window) {
	cmdBuffer->startRecording();

	VkRect2D renderArea{};
	renderArea.offset.x = 0;
	renderArea.offset.y = 0;
	renderArea.extent = window->getSurfaceSize();

	std::array<VkClearValue, 2> clearValues = this->getNewClearValues();

	//Napravimo renderPassBeginInfo (ovo u klasu staviti)
	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = window->getRenderPass()->getRenderPass();
	renderPassBeginInfo.framebuffer = window->getActiveFrameBuffer()->getActiveFrameBuffer(window->getSwapchain()->getActiveImageSwapchain());
	renderPassBeginInfo.renderArea = renderArea;
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();

	//Zapocni render pass
	vkCmdBeginRenderPass(cmdBuffer->getCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdEndRenderPass(cmdBuffer->getCommandBuffer());

	cmdBuffer->endRecording();
}