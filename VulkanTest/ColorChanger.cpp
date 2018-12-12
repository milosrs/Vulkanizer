#include "pch.h"
#include "ColorChanger.h"

ColorChanger::ColorChanger(MainWindow* window, Renderer* renderer) : Scene(window, renderer)
{
	this->clearValues = std::array<VkClearValue, 2> {};
}

std::array<VkClearValue, 2> ColorChanger::getNewClearValues() {
	colorRotator += 0.000000001;

	clearValues[0].depthStencil.depth = 0.0f;
	clearValues[0].depthStencil.stencil = 0;
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };

	clearValues[1].depthStencil.depth = 0.0f;
	clearValues[1].depthStencil.stencil = 0.0f;
	clearValues[1].color = { getColor(CIRCLE_THIRD_1), getColor(CIRCLE_THIRD_2), getColor(CIRCLE_THIRD_3), 0.0f };

	return clearValues;
}

float ColorChanger::getColor(float circleNo) {
	return std::sin(colorRotator + circleNo) * 0.5 + 0.5;
}

void ColorChanger::render(VkViewport* viewport) {
	VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	while (!glfwWindowShouldClose(window->getWindowPTR())) {
		vkWaitForFences(renderer->getDevice(), 1, &this->fences[frameCount], VK_TRUE, std::numeric_limits<uint64_t>::max());
		vkResetFences(renderer->getDevice(), 1, &this->fences[frameCount]);

		VkSemaphore imageAcquiredSemaphore = this->imageAvaiableSemaphores[frameCount];
		VkSemaphore renderSemaphore = this->renderFinishedSemaphores[frameCount];
		VkFence fence = this->fences[frameCount];
		CommandBufferSemaphoreInfo renderSemaphoreInfo(true, renderSemaphore, &stage);
		CommandBufferSemaphoreInfo imageSemaphoreInfo(true, imageAcquiredSemaphore, &stage);
		bool isSubmitted = false;
		glfwPollEvents();
		util->printFPS();
		window->beginRender(imageAcquiredSemaphore);
		
		

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

void ColorChanger::recordFrameBuffer() {
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

}