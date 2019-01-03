#include "pch.h"
#include "RenderObject.h"
#define USES_UNIFORM_BUFFER true

RenderObject::RenderObject(MainWindow* window, Renderer* renderer, std::string name)
{
	this->vertices = std::make_unique<Vertices>();
	this->name = name;
	this->device = renderer->getDevice();
	this->pMemprops = renderer->getPhysicalDeviceMemoryPropertiesPTR();
	this->memprops = renderer->getPhysicalDeviceMemoryProperties();
	this->window = window;
	this->renderer = renderer;

	this->clearValues.resize(2);
	this->clearValues[0] = { 0.2f, 0.2f, 0.2f, 1.0f };			//Background
	this->clearValues[1] = { 1.0f, 0.0f };						//Depth stencil

	this->descriptorHandler = std::make_unique<DescriptorHandler>(device, window->getPipelinePTR()->getDescriptorSetLayout(),
		static_cast<uint32_t>(window->getSwapchain()->getImageViews().size()));

	createSyncObjects();
}

RenderObject::~RenderObject()
{
	deleteSyncObjects();
}

void RenderObject::createSyncObjects() {
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

void RenderObject::deleteSyncObjects()
{
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(renderer->getDevice(), imageAvaiableSemaphores[i], nullptr);
		vkDestroySemaphore(renderer->getDevice(), renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(renderer->getDevice(), fences[i], nullptr);
	}
}

void RenderObject::prepareObject(VkCommandPool cmdPool, VkQueue queue)
{
	if (vertices != nullptr) {
		VkDeviceSize verticesSize = sizeof(vertices->getVertices()[0]) * vertices->getVertices().size();
		VkDeviceSize indicesSize = sizeof(vertices->getIndices()[0]) * vertices->getIndices().size();

		if (texturePath.length() > 0 && mode > 0) {
			this->texture = std::make_unique<Texture>(device, pMemprops, VK_FORMAT_R8G8B8A8_UNORM, texturePath, mode);
			texture->beginCreatingTexture(cmdPool, queue);
		}

		this->vertexBuffer = std::make_unique<VertexBuffer>(device, memprops, verticesSize);
		this->indexBuffer = std::make_unique<IndexBuffer>(device, memprops, indicesSize);

		for (auto i = 0; i < window->getSwapchain()->getImageViews().size(); ++i) {
			uniformBuffers.push_back(new UniformBuffer(device, memprops));
		}

		vertexBuffer->fillBuffer(vertices->getVertices());
		indexBuffer->fillBuffer(vertices->getIndices());
		descriptorHandler->createDescriptorSets(uniformBuffers, texture->getSampler(), texture->getTextureImageView());

		isPrepared = true;
	}
	else {
		throw new std::runtime_error("Vertices of an object cant be null.");
	}
}

void RenderObject::render(VkViewport* viewport) {
	VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	window->setupPipeline(this, USES_UNIFORM_BUFFER);

	if (isObjectReadyToRender()) {
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
			uniformBuffers[activeImageIndex]->update(aspect, nearPlane, farPlane);

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
	else {
		if (this->name == "") {
			throw new std::runtime_error("You did not prepare an object for rendering! Object name unset!");
		}
		else {
			throw new std::runtime_error("You did not prepare an object for rendering! Object name: " + this->name);
		}
	}
}

void RenderObject::setName(std::string name)
{
	this->name = name;
}

void RenderObject::setTextureParams(std::string path, unsigned int mode)
{
	this->texturePath = path;
	this->mode = mode;
}

std::string RenderObject::getName()
{
	return name;
}

bool RenderObject::isObjectReadyToRender()
{
	return isPrepared && this->name != "";
}

IndexBuffer * RenderObject::getIndexBuffer()
{
	return indexBuffer.get();
}

VertexBuffer * RenderObject::getVertexBuffer()
{
	return vertexBuffer.get();
}

Texture * RenderObject::getTexture()
{
	return texture.get();
}

DescriptorHandler * RenderObject::getDescriptorHandler()
{
	return this->descriptorHandler.get();
}

Vertices* RenderObject::getVertices()
{
	return this->vertices.get();
}

std::vector<UniformBuffer*> RenderObject::getUniformBuffers()
{
	return uniformBuffers;
}

std::vector<VkClearValue>* RenderObject::getClearValues()
{
	return &clearValues;
}