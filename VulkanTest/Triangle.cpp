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
		glfwPollEvents();
		vkWaitForFences(renderer->getDevice(), 1, &this->fences[frameCount], VK_TRUE, std::numeric_limits<uint64_t>::max());
		vkResetFences(renderer->getDevice(), 1, &this->fences[frameCount]);

		VkSemaphore imageAcquiredSemaphore = this->imageAvaiableSemaphores[frameCount];
		VkSemaphore renderSemaphore = this->renderFinishedSemaphores[frameCount];
		VkFence fence = this->fences[frameCount];
		bool isSubmitted = false;
		
		float aspect = window->getSurfaceCapatibilities().currentExtent.width / (float)window->getSurfaceCapatibilities().currentExtent.height;
		float nearPlane = 0.1f;
		float farPlane = 10.0f;

		window->beginRender(imageAcquiredSemaphore);

		uint32_t activeImageIndex = window->getSwapchain()->getActiveImageSwapchain();
		CommandBufferSemaphoreInfo renderSemaphoreInfo(true, renderSemaphore, &stage);
		CommandBufferSemaphoreInfo imageSemaphoreInfo(true, imageAcquiredSemaphore, &stage);

				
		window->getDescriptorHandler()->updateDescriptorSets();
		window->getUniformBuffers()[activeImageIndex]->update(aspect, nearPlane, farPlane);


		isSubmitted = window->getCommandHandler()->submitQueue(window->getSwapchain()->getActiveImageSwapchain(),
			renderer->getQueueIndices()->getQueue(), &imageSemaphoreInfo, &renderSemaphoreInfo, &fence);

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

void Triangle::recordFrameBuffer()
{
	
}