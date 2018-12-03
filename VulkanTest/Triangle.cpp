#include "pch.h"
#include "Triangle.h"
#define USES_UNIFORM_BUFFER true

Triangle::Triangle(MainWindow* window, Renderer* renderer) : Scene(window, renderer)
{
	this->vertices = std::make_shared<Vertices>();
	vertices->setVertices({
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		});
	vertices->setIndices({ 0, 1, 2, 2, 3, 0 });
}

void Triangle::render(VkViewport* viewport) {
	VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	window->setupPipeline(vertices, USES_UNIFORM_BUFFER);

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

		uint32_t activeImageIndex = window->getSwapchain()->getActiveImageSwapchain();
		float aspect = window->getSurfaceCapatibilities().currentExtent.width / 
			(float)window->getSurfaceCapatibilities().currentExtent.height;
		float nearPlane = 0.1f;
		float farPlane = 10.0f;

		for (CommandBuffer* cmdBuf : window->getCommandBuffers()) {
			if (cmdBuf->getType() == CommandBufferType::GRAPHICS && cmdBuf == cmdBuffer) {
				cmdBuf->allocateCommandBuffer();
				cmdBuf->startCommandBuffer(window->getPipelinePTR()->getViewportPTR());
				recordFrameBuffer(cmdBuf);
				cmdBuffer->endCommandBuffer();

				window->getUniformBuffers()[activeImageIndex]->update(activeImageIndex, aspect, nearPlane, farPlane);

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

void Triangle::recordFrameBuffer(CommandBuffer * cmdBuffer)
{
	VkCommandBuffer buffer = cmdBuffer->getCommandBuffer();
	VkRect2D renderArea{};
	renderArea.offset.x = 0;
	renderArea.offset.y = 0;
	renderArea.extent = window->getSurfaceSize();

	VkClearValue clearValues = { 0.2f, 0.1f, 0.6f, 1.0f };

	//Napravimo renderPassBeginInfo (ovo u klasu staviti)
	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = window->getRenderPass()->getRenderPass();
	renderPassBeginInfo.framebuffer = window->getActiveFrameBuffer()->getActiveFrameBuffer(window->getSwapchain()->getActiveImageSwapchain());
	renderPassBeginInfo.renderArea = renderArea;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearValues;

	window->getRenderPass()->beginRenderPass(buffer, &renderPassBeginInfo);

	window->bindPipeline(buffer);
	window->draw(buffer, true);

	window->getRenderPass()->endRenderPass(buffer);
}