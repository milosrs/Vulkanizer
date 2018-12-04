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

	if (uniform) {
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

/*AKO HOCES DA SE ZLOPATIS I DA NE KORISTIS GLFW, ODKOMENTARISI KOD ISPOD!*/
/*Ispod su platformski zavisne definicije metoda za ovu klasu*/

/*
#ifdef VK_USE_PLATFORM_WIN32_KHR
// Microsoft Windows specific versions of window functions
LRESULT CALLBACK WindowsEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	MainWindow * window = reinterpret_cast<MainWindow*>(
		GetWindowLongPtrW(hWnd, GWLP_USERDATA));

	switch (uMsg) {
	case WM_CLOSE:
		window->close();
		return 0;
	case WM_SIZE:
		// we get here if the window has changed size, we should rebuild most
		// of our window resources before rendering to this window again.
		// ( no need for this because our window sizing by hand is disabled )
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

uint64_t	MainWindow::win32_class_id_counter = 0;

void MainWindow::InitOSWindow()
{
	WNDCLASSEX win_class{};
	assert(sizeX > 0);
	assert(sizeY > 0);

	win32_instance = GetModuleHandle(nullptr);
	win32_class_name = name + "_" + std::to_string(win32_class_id_counter);

	win32_class_id_counter++;

	// Initialize the window class structure:
	win_class.cbSize = sizeof(WNDCLASSEX);
	win_class.style = CS_HREDRAW | CS_VREDRAW;
	win_class.lpfnWndProc = WindowsEventHandler;
	win_class.cbClsExtra = 0;
	win_class.cbWndExtra = 0;
	win_class.hInstance = win32_instance; // hInstance
	win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	win_class.lpszMenuName = NULL;
	win_class.lpszClassName = util->convertCharArrayToLPCWSTR(win32_class_name.c_str());
	win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	// Register window class:
	if (!RegisterClassEx(&win_class)) {
		// It didn't work, so try to give a useful error:
		assert(0 && "Cannot create a window in which to draw!\n");
		fflush(stdout);
		std::exit(-1);
	}

	DWORD ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	// Create window with the registered class:
	RECT wr = { 0, 0, LONG(surfaceX), LONG(surfaceY) };
	AdjustWindowRectEx(&wr, style, FALSE, ex_style);
	win32_window = CreateWindowEx(0,
		util->convertCharArrayToLPCWSTR(win32_class_name.c_str()),		// class name
		util->convertCharArrayToLPCWSTR(_window_name.c_str()),			// app name
		style,							// window style
		CW_USEDEFAULT, CW_USEDEFAULT,	// x/y coords
		wr.right - wr.left,				// width
		wr.bottom - wr.top,				// height
		NULL,							// handle to parent
		NULL,							// handle to menu
		win32_instance,				// hInstance
		NULL);							// no extra parameters
	if (!win32_window) {
		// It didn't work, so try to give a useful error:
		assert(1 && "Cannot create a window in which to draw!\n");
		fflush(stdout);
		std::exit(-1);
	}
	SetWindowLongPtr(win32_window, GWLP_USERDATA, (LONG_PTR)this);

	ShowWindow(win32_window, SW_SHOW);
	SetForegroundWindow(win32_window);
	SetFocus(win32_window);
}

void MainWindow::DeinitOSWindow()
{
	DestroyWindow(win32_window);
	UnregisterClass(util->convertCharArrayToLPCWSTR(win32_class_name.c_str()), win32_instance);
}

void MainWindow::UpdateOSWindow()
{
	MSG msg;
	if (PeekMessage(&msg, win32_window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void MainWindow::InitOSSurface() {
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hinstance = win32_instance;
	createInfo.hwnd = win32_window;

	renderer->getDebugCallbackCreateInfo();
	vkCreateWin32SurfaceKHR(renderer->getInstance(), &createInfo, nullptr, &surfaceKHR);
}

#else

#endif

#if VK_USE_PLATFORM_XCB_KHR

void MainWindow::InitOSWindow()
{
	// create connection to X11 server
	const xcb_setup_t		*	setup = nullptr;
	xcb_screen_iterator_t		iter;
	int							screen = 0;

	xcb_connection = xcb_connect(nullptr, &screen);
	if (xcb_connection == nullptr) {
		std::cout << "Cannot find a compatible Vulkan ICD.\n";
		exit(-1);
	}

	setup = xcb_get_setup(xcb_connection);
	iter = xcb_setup_roots_iterator(setup);
	while (screen-- > 0) {
		xcb_screen_next(&iter);
	}
	xcb_screen = iter.data;

	// create window
	VkRect2D dimensions = { { 0, 0 }, { _surface_size_x, _surface_size_y } };

	assert(dimensions.extent.width > 0);
	assert(dimensions.extent.height > 0);

	uint32_t value_mask, value_list[32];

	xcb_window = xcb_generate_id(xcb_connection);

	value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	value_list[0] = xcb_screen->black_pixel;
	value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE;

	xcb_create_window(xcb_connection, XCB_COPY_FROM_PARENT, xcb_window,
		xcb_screen->root, dimensions.offset.x, dimensions.offset.y,
		dimensions.extent.width, dimensions.extent.height, 0,
		XCB_WINDOW_CLASS_INPUT_OUTPUT, xcb_screen->root_visual,
		value_mask, value_list);

	//Magic code that will send notification when window is destroyed 
	xcb_intern_atom_cookie_t cookie =
		xcb_intern_atom(xcb_connection, 1, 12, "WM_PROTOCOLS");
	xcb_intern_atom_reply_t *reply =
		xcb_intern_atom_reply(xcb_connection, cookie, 0);

	xcb_intern_atom_cookie_t cookie2 =
		xcb_intern_atom(xcb_connection, 0, 16, "WM_DELETE_WINDOW");
	xcb_atom_window_reply =
		xcb_intern_atom_reply(xcb_connection, cookie2, 0);

	xcb_change_property(xcb_connection, XCB_PROP_MODE_REPLACE, xcb_window,
		(*reply).atom, 4, 32, 1,
		&(*xcb_atom_window_reply).atom);
	free(reply);

	xcb_map_window(xcb_connection, xcb_window);

	// Force the x/y coordinates to 100,100 results are identical in consecutive
	// runs
	const uint32_t coords[] = { 100, 100 };
	xcb_configure_window(xcb_connection, xcb_window,
		XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
	xcb_flush(xcb_connection);

	
	xcb_generic_event_t *e;
	while( ( e = xcb_wait_for_event( xcb_connection ) ) ) {
		if( ( e->response_type & ~0x80 ) == XCB_EXPOSE )
			break;
	}
	
}

void MainWindow::DeinitOSWindow()
{
	xcb_destroy_window(xcb_connection, xcb_window);
	xcb_disconnect(xcb_connection);
	xcb_window = 0;
	xcb_connection = nullptr;
}

void MainWindow::UpdateOSWindow()
{
	auto event = xcb_poll_for_event(xcb_connection);

	// if there is no event, event will be NULL
	// need to check for event == NULL to prevent segfault
	if (!event)
		return;

	switch (event->response_type & ~0x80) {
	case XCB_CLIENT_MESSAGE:
		if (((xcb_client_message_event_t*)event)->data.data32[0] == xcb_atom_window_reply->atom) {
			Close();
		}
		break;
	default:
		break;
	}
	free(event);
}

void MainWindow::InitOSSurface()
{
}

/*
void MainWindow::InitOSSurface()
{
	VkXcbSurfaceCreateInfoKHR create_info {};
	create_info.sType			= VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	create_info.connection		= xcb_connection;
	create_info.window			= xcb_window;
	ErrorCheck( vkCreateXcbSurfaceKHR( _renderer->GetVulkanInstance(), &create_info, nullptr, &_surface ) );
}
#endif
*/