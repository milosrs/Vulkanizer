#include "pch.h"
#include "Scene.h"
#define USES_UNIFORM_BUFFER true

Scene::Scene(MainWindow* window, Renderer* renderer)
{
	this->window = window;
	this->renderer = renderer;

	createSyncObjects();
}

Scene::~Scene()
{
	deleteSyncObjects();
}

void Scene::createSyncObjects() {
	imageAvaiableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	fences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fcreateInfo{};
	fcreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fcreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		Util::ErrorCheck(vkCreateSemaphore(renderer->getDevice(), &createInfo, nullptr, &imageAvaiableSemaphores[i]));
		Util::ErrorCheck(vkCreateSemaphore(renderer->getDevice(), &createInfo, nullptr, &renderFinishedSemaphores[i]));
		Util::ErrorCheck(vkCreateFence(renderer->getDevice(), &fcreateInfo, nullptr, &fences[i]));
	}
}

void Scene::deleteSyncObjects()
{
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(renderer->getDevice(), imageAvaiableSemaphores[i], nullptr);
		vkDestroySemaphore(renderer->getDevice(), renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(renderer->getDevice(), fences[i], nullptr);
	}
}

void Scene::render(VkViewport* viewport) {
	VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	window->setupPipeline(vertices, clearValues, USES_UNIFORM_BUFFER);

	while (!glfwWindowShouldClose(window->getWindowPTR())) {
		glfwPollEvents();

		VkSemaphore imageAcquiredSemaphore = this->imageAvaiableSemaphores[frameCount];
		VkSemaphore renderSemaphore = this->renderFinishedSemaphores[frameCount];
		VkFence fence = this->fences[frameCount];
		uint32_t activeImageIndex;
		bool isSubmitted = false;
		float aspect = window->getSurfaceCapatibilities().currentExtent.width / (float)window->getSurfaceCapatibilities().currentExtent.height;
		float nearPlane = 0.1f;
		float farPlane = 10.0f;

		vkWaitForFences(renderer->getDevice(), 1, &this->fences[frameCount], VK_TRUE, std::numeric_limits<uint64_t>::max());

		window->beginRender(imageAcquiredSemaphore);
		activeImageIndex = window->getSwapchain()->getActiveImageSwapchain();
		window->getUniformBuffers()[activeImageIndex]->update(aspect, nearPlane, farPlane);

		CommandBufferSemaphoreInfo renderSemaphoreInfo(true, renderSemaphore, &stage);
		CommandBufferSemaphoreInfo imageSemaphoreInfo(true, imageAcquiredSemaphore, &stage);

		vkResetFences(renderer->getDevice(), 1, &this->fences[frameCount]);

		isSubmitted = window->getCommandHandler()->submitQueue(activeImageIndex, renderer->getQueueIndices()->getQueue(),
			&imageSemaphoreInfo, &renderSemaphoreInfo, &fence);

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