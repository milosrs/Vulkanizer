#include "pch.h"
#include "MainWindow.h"
#include "Renderer.h"
#include "CommandBufferHandler.h"

MainWindow::MainWindow(Renderer* renderer, uint32_t sizeX, uint32_t sizeY, std::string windowName) {
	this->renderer = renderer;
	this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->name = windowName;

	InitOSWindow();
	InitOSSurface();
}

MainWindow::MainWindow(const MainWindow &)
{
}

void MainWindow::continueInitialization(Renderer* renderer) {
	this->renderer = renderer;

	createData();
}

MainWindow::~MainWindow()
{
	destroySwapchainDependencies();
	DestroySurface();
	renderer->_DeinitInstance();
	DeinitOSWindow();
}

void MainWindow::destroySwapchainDependencies() {
	vkQueueWaitIdle(renderer->getQueue());

	FrameBuffer* fbptr = frameBuffer.release();
	fbptr->~FrameBuffer();

	RenderPass* rpptr = renderPass.release();
	rpptr->~RenderPass();

	Swapchain* sptr = swapchain.release();
	sptr->~Swapchain();
}

void MainWindow::createData()
{
	int width = 0, height = 0;									//Minimizacija, pauziramo render dok se ne vrati slika na povrsinu ekrana.
	VkExtent2D scissorsExtent;

	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	InitSurface();

	swapchain = std::make_unique<Swapchain>(this, renderer);
	renderPass = std::make_unique<RenderPass>(renderer, swapchain->getDepthStencilFormat(), this->surfaceFormat);
	scissorsExtent = surfaceCapatibilities.currentExtent;
	pipeline = std::make_unique<Pipeline>(renderer->getDevice(), renderer->getPhysicalDeviceMemoryProperties(),
		renderPass->getRenderPassPTR(), (float)width, (float)height, scissorsExtent);
	frameBuffer = std::make_unique<FrameBuffer>(renderer, swapchain->getSwapchainImageCount(), swapchain->getImageViews(),
		renderPass->getRenderPass(), this->getSurfaceSize());
	descriptorHandler = std::make_unique<DescriptorHandler>(renderer->getDevice(), pipeline->getDescriptorSetLayout(), 
															static_cast<uint32_t>(swapchain->getImageViews().size()));
	commandBufferHandler = std::make_unique<CommandBufferHandler>(renderer->getQueueIndices()->getGraphicsFamilyIndex(),
		renderer->getDevice(), this);
}


void MainWindow::setupPipeline(std::shared_ptr<Vertices> vertices, bool uniform)
{
	this->texture = std::make_unique<Texture>(renderer->getDevice(), renderer->getPhysicalDeviceMemoryPropertiesPTR(),
		VK_FORMAT_R8G8B8A8_UNORM, "../Textures/riki.jpg", 4);
	this->vertexBuffer = std::make_unique<VertexBuffer>(renderer->getDevice(), renderer->getPhysicalDeviceMemoryProperties(),
		vertices);
	this->indexBuffer = std::make_unique<IndexBuffer>(renderer->getDevice(), renderer->getPhysicalDeviceMemoryProperties(),
		vertices->getIndices());

	texture->beginCreatingTexture(commandBufferHandler->getCommandPool(), 
		renderer->getQueueIndices()->getQueues()[renderer->getQueueIndices()->getGraphicsFamilyIndex()]);

	
	for (auto i = 0; i < swapchain->getImageViews().size(); ++i) {
		uniformBuffers.push_back(new UniformBuffer(renderer->getDevice(), renderer->getPhysicalDeviceMemoryProperties()));
	}

	descriptorHandler->createDescriptorSets(uniformBuffers, texture->getSampler(), texture->getTextureImageView());
	
	vertexBuffer->fillBuffer();
	indexBuffer->fillBuffer();

	commandBufferHandler->createDrawingCommandBuffers(static_cast<uint32_t>(frameBuffer->getFrameBuffers().size()));
}

void MainWindow::bindPipeline(VkCommandBuffer cmdBuffer)
{
	this->pipeline->bindPipeline(cmdBuffer, this->vertexBuffer.get(), this->indexBuffer.get());
}

void MainWindow::InitSurface() {
	VkPhysicalDevice device = renderer->getGpu();
	
	vkGetPhysicalDeviceSurfaceSupportKHR(device, renderer->getQueueIndices()->getGraphicsFamilyIndex(), this->surfaceKHR, &isWSISupported);

	if (!isWSISupported) {
		assert(0 && "WSI not supported.");
		exit(-1);
	}

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, this->surfaceKHR, &surfaceCapatibilities);

	if (surfaceCapatibilities.currentExtent.width < UINT32_MAX) {											//Ako je > exception
		sizeX = surfaceCapatibilities.currentExtent.width;
		sizeY = surfaceCapatibilities.currentExtent.height;
	}

	choosePreferedFormat();
}

void MainWindow::choosePreferedFormat() {
	VkPhysicalDevice device = renderer->getGpu();
	uint32_t formatCount = 0;
	bool surfaceFormatFound = false;

	vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->surfaceKHR, &formatCount, nullptr);					//Kakve frame buffere treba da napravimo

	if (formatCount == 0) {
		assert(0 && "Surface format missing.");
		exit(-1);
	}

	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->surfaceKHR, &formatCount, surfaceFormats.data());	//Kakve frame buffere treba da napravimo

	if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {					//Surface-u nije bitan format
		this->surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;									//SRGB je prostor boja koji je najbolje prilagodjen ljudskom oku. NONLINEAR je da bi smo mogli da iskoristimo bitove sRGB boja na najbolji moguci nacin.
		this->surfaceFormat.format = VK_FORMAT_B8G8R8_UNORM;												//Format koji koristimo je BGR 8bit. Manipulisanje sRGB bojama je dosta zahtevno i izlazi van okvira vulkana.
		surfaceFormatFound = true;
	}
	else {
		for (const VkSurfaceFormatKHR& format : surfaceFormats) {
			if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_UNORM) {
				this->surfaceFormat = format;
				surfaceFormatFound = true;
				break;
			}
		}
	}

	if (!surfaceFormatFound) {
		this->surfaceFormat = surfaceFormats[0];
	}
}

void MainWindow::beginRender(VkSemaphore semaphoreToWait)
{
	VkResult result;

	//Kada funkcija vrati sliku, moze da bude koriscena od strane prezentacionog endzina. Semafor i/ili Ograda ce nam reci kada je prez.endz. gotov sa poslom. Nesto kao mutex.
	result = vkAcquireNextImageKHR(renderer->getDevice(), this->swapchain->getSwapchain(), 
		std::numeric_limits<uint64_t>::max(), semaphoreToWait, 
		VK_NULL_HANDLE, this->swapchain->getActiveImageSwapchainPTR());

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS) {
		Util::ErrorCheck(result);
	}
}

void MainWindow::endRender(std::vector<VkSemaphore> waitSemaphores)
{
	VkResult presentResult = VkResult::VK_RESULT_MAX_ENUM;
	VkSwapchainKHR swapchains[] = { swapchain->getSwapchain() };

	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pWaitSemaphores = waitSemaphores.data();
	presentInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = this->swapchain->getActiveImageSwapchainPTR();
	presentInfo.pResults = &presentResult;

	Util::ErrorCheck(vkQueuePresentKHR(renderer->getQueueIndices()->getQueue(), &presentInfo));
}

void MainWindow::recreateSwapchain()
{
	destroySwapchainDependencies();

	createData();
}

Renderer* MainWindow::getRenderer()
{
	return this->renderer;
}

RenderPass* MainWindow::getRenderPass()
{
	return this->renderPass.get();
}

FrameBuffer* MainWindow::getActiveFrameBuffer()
{
	return this->frameBuffer.get();
}

Swapchain* MainWindow::getSwapchain()
{
	return this->swapchain.get();
}

DescriptorHandler * MainWindow::getDescriptorHandler()
{
	return descriptorHandler.get();
}

GLFWwindow * MainWindow::getWindowPTR()
{
	return this->window;
}

Pipeline * MainWindow::getPipelinePTR()
{
	return this->pipeline.get();
}

IndexBuffer * MainWindow::getIndexBufferPTR()
{
	return indexBuffer.get();
}

VertexBuffer* MainWindow::getVertexBufferPTR()
{
	return vertexBuffer.get();
}

CommandBufferHandler * MainWindow::getCommandHandler()
{
	return this->commandBufferHandler.get();
}

std::vector<UniformBuffer*> MainWindow::getUniformBuffers()
{
	return this->uniformBuffers;
}

VkSurfaceKHR MainWindow::getSurface()
{
	return this->surfaceKHR;
}

VkSurfaceKHR * MainWindow::getSurfacePTR()
{
	return &this->surfaceKHR;
}

VkExtent2D MainWindow::getSurfaceSize()
{
	return { sizeX, sizeY };
}

VkSurfaceCapabilitiesKHR MainWindow::getSurfaceCapatibilities()
{
	return this->surfaceCapatibilities;
}

VkBool32 MainWindow::getIsWSISupported()
{
	return this->isWSISupported;
}

VkSurfaceFormatKHR MainWindow::getSurfaceFormat()
{
	return this->surfaceFormat;
}

void MainWindow::DestroySurface() {
	vkDestroySurfaceKHR(renderer->getInstance(), this->surfaceKHR, nullptr);
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
	app->windowResized = true;
}

void MainWindow::InitOSWindow()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);													//Ovo kaze biblioteci da aplikacija nije pisana u OpenGL/ES
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);														//Za sada, promena velicine prozora nije moguca
	this->window = glfwCreateWindow(this->sizeX, this->sizeY, "Hello world!", nullptr, nullptr);	//4-i param: Koji monitor je u pitanju (sada je default)  5-i param: Samo za OpenGL aplikacije
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void MainWindow::DeinitOSWindow()
{
	glfwDestroyWindow(this->window);
	glfwTerminate();
}

void MainWindow::InitOSSurface()
{
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	surfaceCreateInfo.hwnd = glfwGetWin32Window(this->window);
	VkResult result = glfwCreateWindowSurface(renderer->getInstance(), this->window, nullptr, &surfaceKHR);
	Util::ErrorCheck(result);
}