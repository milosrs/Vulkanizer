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

void ColorChanger::render(VkViewport* viewport) {
	VkSemaphore renderSemaphore = nullptr;
	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	while (!glfwWindowShouldClose(window->getWindowPTR())) {
		glfwPollEvents();
		util->printFPS();
		window->beginRender();

		vkCreateSemaphore(renderer->getDevice(), &semaphoreCreateInfo, nullptr, &renderSemaphore);
		CommandBufferSemaphoreInfo semaphoreInfo(true, renderSemaphore, &stage);

		cmdBuffer->allocateCommandBuffer();
		cmdBuffer->startCommandBuffer(viewport);

		recordFrameBuffer(cmdBuffer, window);

		cmdBuffer->endCommandBuffer();
		cmdBuffer->submitQueue(renderer->getQueueIndices()->getQueue(), window->getImageAcquiredSemaphore(), &semaphoreInfo);
		

		window->endRender({ renderSemaphore });
	}

	vkQueueWaitIdle(renderer->getQueue());
	vkDestroySemaphore(renderer->getDevice(), renderSemaphore, nullptr);
}

void ColorChanger::recordFrameBuffer(CommandBuffer* cmdBuffer, MainWindow* window) {
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

	window->getRenderPass()->beginRenderPass(window->getActiveFrameBuffer()->getActiveFrameBuffer(window->getSwapchain()->getActiveImageSwapchain()), 
		window->getSurfaceSize(), cmdBuffer->getCommandBuffer(), clearValues);
}