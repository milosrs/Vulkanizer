#include "pch.h"
#include "Triangle.h"


Triangle::Triangle(MainWindow* window, Renderer* renderer) : Scene(window, renderer)
{
}

void Triangle::render(VkViewport* viewport) {
	VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	while (!glfwWindowShouldClose(window->getWindowPTR())) {
		vkWaitForFences(renderer->getDevice(), 1, &this->fences[frameCount], VK_TRUE, std::numeric_limits<uint64_t>::max());
		vkResetFences(renderer->getDevice(), 1, &this->fences[frameCount]);

		VkSemaphore imageAcquiredSemaphore = this->imageAvaiableSemaphores[frameCount];
		VkSemaphore renderSemaphore = this->renderFinishedSemaphores[frameCount];
		VkFence fence = this->fences[frameCount];
		CommandBuffer* cmdBuffer = window->getCommandBuffers()[window->getSwapchain()->getActiveImageSwapchain()];
		CommandBufferSemaphoreInfo renderSemaphoreInfo(true, renderSemaphore, &stage);
		CommandBufferSemaphoreInfo imageSemaphoreInfo(true, imageAcquiredSemaphore, &stage);
		bool isSubmitted = false;

		glfwPollEvents();
		util->printFPS();
		window->beginRender(imageAcquiredSemaphore);

		for (CommandBuffer* cmdBuf : window->getCommandBuffers()) {
			if (cmdBuffer == cmdBuf) {
				recordFrameBuffer(cmdBuf, window);
				cmdBuffer->endCommandBuffer();
				isSubmitted = cmdBuffer->submitQueue(renderer->getDevice(), renderer->getQueueIndices()->getQueue(),
														&imageSemaphoreInfo, &renderSemaphoreInfo, &fence);
			}
		}

		if (!isSubmitted) {
			window->recreateSwapchain();
			continue;
		}

		window->endRender({ renderSemaphore });

		frameCount = (frameCount + 1) % MAX_FRAMES_IN_FLIGHT;

		vkQueueWaitIdle(renderer->getQueueIndices()->getQueue());
	}

	vkDeviceWaitIdle(renderer->getDevice());
}

void Triangle::recordFrameBuffer(CommandBuffer * cmdBuffer, MainWindow * window)
{
	VkRect2D renderArea{};
	renderArea.offset.x = 0;
	renderArea.offset.y = 0;
	renderArea.extent = window->getSurfaceSize();

	VkClearValue clearValues = { 0.0f, 0.0f, 0.0f, 1.0f };

	//Napravimo renderPassBeginInfo (ovo u klasu staviti)
	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = window->getRenderPass()->getRenderPass();
	renderPassBeginInfo.framebuffer = window->getActiveFrameBuffer()->getActiveFrameBuffer(window->getSwapchain()->getActiveImageSwapchain());
	renderPassBeginInfo.renderArea = renderArea;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearValues;

	window->getRenderPass()->beginRenderPass(cmdBuffer->getCommandBuffer(), &renderPassBeginInfo);
	window->getPipelinePTR()->bindPipeline(cmdBuffer->getCommandBuffer());
	window->draw(cmdBuffer->getCommandBuffer());
	window->getRenderPass()->endRenderPass(cmdBuffer->getCommandBuffer());
}
