#include "pch.h"
#include "MainWindow.h"
#include "Renderer.h"

MainWindow::MainWindow(Renderer* renderer, uint32_t sizeX, uint32_t sizeY, std::string windowName) {
	this->renderer = renderer;
	this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->name = windowName;
	this->util = &Util::instance();

	InitOSWindow();
	InitOSSurface();
}

MainWindow::MainWindow(const MainWindow &)
{
}

void MainWindow::continueInitialization(Renderer* renderer) {
	this->renderer = renderer;
	this->cmdPool = std::make_unique<CommandPool>(renderer->getQueueIndices()->getGraphicsFamilyIndex(), renderer->getDevicePTR());
	this->transferCommandPool = std::make_unique<CommandPool>(renderer->getQueueIndices()->getGraphicsFamilyIndex(), renderer->getDevicePTR(),
																VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);

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
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	InitSurface();

	swapchain = std::make_unique<Swapchain>(this, renderer);

	std::vector<VkImageView> attachments = { swapchain->getDepthStencilImageView() };

	renderPass = std::make_unique<RenderPass>(renderer, swapchain->getDepthStencilFormat(), this->surfaceFormat);
	frameBuffer = std::make_unique<FrameBuffer>(renderer, swapchain->getSwapchainImageCount(), swapchain->getImageViews(),
		renderPass->getRenderPass(), this->getSurfaceSize());

	VkExtent2D scissorsExtent = surfaceCapatibilities.currentExtent;

	pipeline = std::make_unique<Pipeline>(renderer->getDevice(), renderer->getPhysicalDeviceMemoryProperties(),
												renderPass->getRenderPassPTR(), width, height, scissorsExtent);
	
	descriptorHandler = std::make_unique<DescriptorHandler>(renderer->getDevice(), pipeline->getDescriptorSetLayout(), 
															swapchain->getImageViews().size());

	for (size_t i = 0; i < frameBuffer->getFrameBuffers().size(); i++) {
		cmdBuffers.push_back(new CommandBuffer(cmdPool->getCommandPool(), renderer->getDevice(), 
												VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, CommandBufferType::GRAPHICS));
	}

	cmdBuffers.push_back(new CommandBuffer(transferCommandPool->getCommandPool(), renderer->getDevice(),
											VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, CommandBufferType::TRANSFER));
}


void MainWindow::setupPipeline(std::shared_ptr<Vertices> vertices, bool uniform)
{
	CommandBuffer* transferBuffer = this->cmdBuffers[cmdBuffers.size() - 1];

	this->indexBuffer = std::make_unique<IndexBuffer>(renderer->getDevice(), renderer->getPhysicalDeviceMemoryProperties(),
														vertices->getIndices());

	this->vertexBuffer = std::make_unique<VertexBuffer>(renderer->getDevice(), renderer->getPhysicalDeviceMemoryProperties(),
														vertices);

	if (!uniform) {
		/*Pravimo Vertex Staging buffer*/
		StagingBuffer<Vertex>* stagingBufferVertex = new StagingBuffer<Vertex>(renderer->getDevice(), renderer->getPhysicalDeviceMemoryProperties(),
			vertexBuffer->getSize());

		stagingBufferVertex->fillBuffer(vertexBuffer->getVertices());

		transferBuffer->copyBuffer(stagingBufferVertex->getBuffer(), vertexBuffer->getBuffer(),
			vertexBuffer->getSize(), renderer->getQueueIndices()->getQueue());

		stagingBufferVertex->~StagingBuffer();

		/*Pravimo Index Staging buffer*/
		StagingBuffer<uint16_t>* stagingBufferIndices = new StagingBuffer<uint16_t>(renderer->getDevice(), renderer->getPhysicalDeviceMemoryProperties(),
			indexBuffer->getSize());

		stagingBufferIndices->fillBuffer(indexBuffer->getIndices());

		transferBuffer = new CommandBuffer(transferCommandPool->getCommandPool(), renderer->getDevice(),
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, CommandBufferType::TRANSFER);

		transferBuffer->copyBuffer(stagingBufferIndices->getBuffer(), indexBuffer->getBuffer(),
			indexBuffer->getSize(), renderer->getQueueIndices()->getQueue());

		stagingBufferIndices->~StagingBuffer();
	}
	else {
		for (auto i = 0; i < swapchain->getImageViews().size(); ++i) {
			uniformBuffers.push_back(new UniformBuffer(renderer->getDevice(), renderer->getPhysicalDeviceMemoryProperties()));
		}

		descriptorHandler->createDescriptorSets(uniformBuffers);
	}
	
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
		util->ErrorCheck(result);
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
	presentInfo.pSwapchains = this->swapchain->getSwapchainPTR();
	presentInfo.pImageIndices = this->swapchain->getActiveImageSwapchainPTR();
	presentInfo.pResults = &presentResult;

	util->ErrorCheck(vkQueuePresentKHR(renderer->getQueueIndices()->getQueue(), &presentInfo));
	std::cout << presentResult << std::endl;
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

std::vector< CommandBuffer*> MainWindow::getCommandBuffers()
{
	return this->cmdBuffers;
}

std::vector<UniformBuffer*> MainWindow::getUniformBuffers()
{
	return this->uniformBuffers;
}

CommandPool * MainWindow::getCommandPoolPTR()
{
	return this->cmdPool.get();
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
	util->ErrorCheck(result);
}


void MainWindow::draw(VkCommandBuffer commandBuffer, bool isIndexed)
{
	if (!isIndexed) {
		vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertexBuffer->getVertices().size()), 1, 0, 0);
	}
	else {
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(vertexBuffer->getIndices().size()), 1, 0, 0, 0);
	}
}