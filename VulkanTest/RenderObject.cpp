#include "pch.h"
#include "RenderObject.h"
#define USES_UNIFORM_BUFFER true

RenderObject::RenderObject(std::string name)
{
	this->window = &MainWindow::getInstance();
	this->renderer = window->getRenderer();
	this->vertices = std::make_unique<Vertices>();
	this->name = name;
	this->device = renderer->getDevice();
	this->pMemprops = renderer->getPhysicalDeviceMemoryPropertiesPTR();
	this->memprops = renderer->getPhysicalDeviceMemoryProperties();
	this->window = &MainWindow::getInstance();
	this->renderer = window->getRenderer();

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

		if (texturePaths.size() > 0 && mode > 0) {
			VkFormat imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
			
			for (const std::string& path : texturePaths) {
				Texture *t = new Texture(device, pMemprops, VK_FORMAT_R8G8B8A8_UNORM, path, mode);
				t->supportsLinearBlitFormat(renderer->getGpu());
				t->beginCreatingTexture(cmdPool, queue);
				this->textures.push_back(t);
			}
		}

		this->vertexBuffer = std::make_unique<VertexBuffer>(device, memprops, verticesSize);
		this->indexBuffer = std::make_unique<IndexBuffer>(device, memprops, indicesSize);

		float aspect = window->getSurfaceCapatibilities().currentExtent.width / (float)window->getSurfaceCapatibilities().currentExtent.height;
		float nearPlane = 0.1f;
		float farPlane = 10.0f;

		for (auto i = 0; i < window->getSwapchain()->getImageViews().size(); ++i) {
			UniformBuffer *ub = new UniformBuffer(device, memprops);
			ub->setViewData(aspect, nearPlane, farPlane);
			uniformBuffers.push_back(ub);
		}

		vertexBuffer->fillBuffer(vertices->getVertices());
		indexBuffer->fillBuffer(vertices->getIndices());
		descriptorHandler->createDescriptorSets(uniformBuffers, textures[0]->getSampler(), 
			textures[0]->getTextureImageView());

		isPrepared = true;
	}
	else {
		throw new std::runtime_error("Vertices of an object cant be null.");
	}
}

void RenderObject::render() {
	VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	window->setupPipeline(this, USES_UNIFORM_BUFFER);

	if (isObjectReadyToRender()) {
		while (!glfwWindowShouldClose(window->getWindowPTR())) {
			glfwPollEvents();

			VkSemaphore imageAcquiredSemaphore = this->imageAvaiableSemaphores[frameCount];
			VkSemaphore renderSemaphore = this->renderFinishedSemaphores[frameCount];
			VkFence fence = this->fences[frameCount];
			bool isSubmitted = false;

			vkWaitForFences(renderer->getDevice(), 1, &this->fences[frameCount], VK_TRUE, std::numeric_limits<uint64_t>::max());

			window->beginRender(imageAcquiredSemaphore);
			activeImageIndex = window->getSwapchain()->getActiveImageSwapchain();
			uniformBuffers[activeImageIndex]->update();

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

void RenderObject::setTextureParams(std::vector<std::string> texturePaths, unsigned int mode)
{
	this->texturePaths = texturePaths;
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

void RenderObject::recreateDescriptorHandler()
{
	this->descriptorHandler = std::make_unique<DescriptorHandler>(device, window->getPipelinePTR()->getDescriptorSetLayout(),
		static_cast<uint32_t>(window->getSwapchain()->getImageViews().size()));
}

void RenderObject::rotate(glm::vec2 mouseDelta)
{
	uniformBuffers[activeImageIndex]->rotate(mouseDelta);
}

IndexBuffer * RenderObject::getIndexBuffer()
{
	return indexBuffer.get();
}

VertexBuffer * RenderObject::getVertexBuffer()
{
	return vertexBuffer.get();
}
std::vector<Texture*> RenderObject::getTextures()
{
	return this->textures;
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

std::vector<std::string> RenderObject::getTexturePaths()
{
	return this->texturePaths;
}
