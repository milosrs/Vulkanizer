#include "pch.h"
#include "MainWindow.h"
#include "Renderer.h"

MainWindow::MainWindow(Renderer* renderer, uint32_t sizeX, uint32_t sizeY, std::string windowName)
{
	this->renderer = renderer;
	this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->name = windowName;

	InitOSWindow();
	InitSurface();
	initSwapchain();
	initSwapchainImgs();
}


MainWindow::~MainWindow()
{
	DestroySurface();
	DeinitOSWindow();
	destroySwapchain();
	destroySwapchainImgs();
}

void MainWindow::close()
{
	window_should_run = false;
}

bool MainWindow::update()
{
	UpdateOSWindow();
	return window_should_run;
}

void MainWindow::InitSurface() {
	VkPhysicalDevice device = renderer->getGpu();
	uint32_t formatCount = 0;
	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);

	InitOSSurface();
	vkGetPhysicalDeviceSurfaceSupportKHR(device, renderer->getGraphicsFamilyIndex(), this->surfaceKHR, &isWSISupported);

	if (!isWSISupported) {
		assert(0 && "WSI not supported.");
		exit(-1);
	}

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, this->surfaceKHR, &surfaceCapatibilities);

	if (surfaceCapatibilities.currentExtent.width < UINT32_MAX) {		//Ako je > exception
		surfaceX = surfaceCapatibilities.currentExtent.width;
		surfaceY = surfaceCapatibilities.currentExtent.height;
	}

	{
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->surfaceKHR, &formatCount, nullptr);	//Kakve frame buffere treba da napravimo
		
		if (formatCount == 0) {
			assert(0 && "Surface format missing.");
			exit(-1);
		}

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->surfaceKHR, &formatCount, surfaceFormats.data());	//Kakve frame buffere treba da napravimo
		
		if (surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {			//Surface-u nije bitan format
			this->surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			this->surfaceFormat.format = VK_FORMAT_B8G8R8_UNORM;
		}
		else {
			this->surfaceFormat = surfaceFormats[0];
		}
	}
}

void MainWindow::initSwapchain() {
	Util& util = Util::instance();

	if (swapchainImageCount > surfaceCapatibilities.maxImageCount) {
		swapchainImageCount = surfaceCapatibilities.maxImageCount;
	}
	if (swapchainImageCount < surfaceCapatibilities.minImageCount) {
		swapchainImageCount = surfaceCapatibilities.minImageCount + 1;
	}

	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;					//Ako nista drugo ne radi, Vulkan garantuje da je ovaj prez. mode dostupan

	{
		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->getGpu(), this->surfaceKHR, &presentModeCount, nullptr);
		std::vector<VkPresentModeKHR> presentModeList(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->getGpu(), this->surfaceKHR, &presentModeCount, presentModeList.data());

		if (std::find(presentModeList.begin(), presentModeList.end(), VK_PRESENT_MODE_MAILBOX_KHR) != presentModeList.end()) {
			presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		}
	}

	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = this->surfaceKHR;
	swapchainCreateInfo.minImageCount = swapchainImageCount;					//Double buffering (Bufferovanje slika display buffera)
	swapchainCreateInfo.imageFormat = this->surfaceFormat.format;
	swapchainCreateInfo.imageColorSpace = this->surfaceFormat.colorSpace;
	swapchainCreateInfo.imageExtent.width = this->surfaceX;
	swapchainCreateInfo.imageExtent.height = this->surfaceY;
	swapchainCreateInfo.imageArrayLayers = 1;									//Koliko slojeva ima slika (1 je obicno renderovanje, 2 je stetoskopsko)
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;			//NE delimo slike izmedju Queue-ova. Paralel znaci da hocemo da delimo.
	swapchainCreateInfo.queueFamilyIndexCount = 0;								//Za exclusive je uvek 0
	swapchainCreateInfo.pQueueFamilyIndices = nullptr;							//Isto ignorisemo za Exclusive
	swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;	
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;		//Alfa kanal SURFACE-a, da li je ona transparentna
	swapchainCreateInfo.presentMode = ;											//Vertical Sync
	swapchainCreateInfo.clipped = VK_TRUE;										//Ukljucujemo clipping, jako bitno za telefone
	swapchainCreateInfo.oldSwapchain = swapchain;								//Ako rekonstruisemo swapchain, pokazivac na stari

	util.ErrorCheck(vkCreateSwapchainKHR(renderer->getDevice(), &swapchainCreateInfo, nullptr, &swapchain));
	util.ErrorCheck(vkGetSwapchainImagesKHR(renderer->getDevice(), swapchain, &swapchainImageCount, nullptr));
}

void MainWindow::destroySwapchain()
{
	vkDestroySwapchainKHR(renderer->getDevice(), swapchain, nullptr);
}

void MainWindow::initSwapchainImgs()
{
	Util& util = Util::instance();
	images.resize(swapchainImageCount);
	imageViews.resize(swapchainImageCount);

	util.ErrorCheck(vkGetSwapchainImagesKHR(renderer->getDevice(), swapchain, &swapchainImageCount, images.data()));

	for (int i = 0; i < swapchainImageCount; i++) {
		VkImageViewCreateInfo imgCreateInfo = {};
		imgCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		imgCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		imgCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		imgCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
		imgCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imgCreateInfo.subresourceRange.baseMipLevel = 0;
		imgCreateInfo.subresourceRange.levelCount = 1;
		imgCreateInfo.subresourceRange.baseArrayLayer = 0;
		imgCreateInfo.subresourceRange.layerCount = 1;
		imgCreateInfo.format = surfaceFormat.format;
		imgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imgCreateInfo.image = images[i];
		imgCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;


		util.ErrorCheck(vkCreateImageView(renderer->getDevice(), &imgCreateInfo, nullptr, &imageViews[i]));
	}
}	

void MainWindow::destroySwapchainImgs()
{
	for (int i = 0; i < imageViews.size(); i++) {
		vkDestroyImageView(renderer->getDevice(), imageViews[i], nullptr);
	}
}

void MainWindow::DestroySurface() {
	vkDestroySurfaceKHR(renderer->getInstance(), this->surfaceKHR, nullptr);
}

/*Ispod su platformski zavisne definicije metoda za ovu klasu*/

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
	Util& util = Util::instance();
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
	win_class.lpszClassName = util.convertCharArrayToLPCWSTR(win32_class_name.c_str());
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
		util.convertCharArrayToLPCWSTR(win32_class_name.c_str()),		// class name
		util.convertCharArrayToLPCWSTR(_window_name.c_str()),			// app name
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
	Util& util = Util::instance();
	DestroyWindow(win32_window);
	UnregisterClass(util.convertCharArrayToLPCWSTR(win32_class_name.c_str()), win32_instance);
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

	/* Magic code that will send notification when window is destroyed */
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

	/*
	xcb_generic_event_t *e;
	while( ( e = xcb_wait_for_event( xcb_connection ) ) ) {
		if( ( e->response_type & ~0x80 ) == XCB_EXPOSE )
			break;
	}
	*/
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
*/

#endif