#include "pch.h"
#include "Swapchain.h"
#include "MainWindow.h"

Swapchain::Swapchain(MainWindow* mainWindow, Renderer* renderer)
{
	util = &Util::instance();
	this->mainWindow = mainWindow;
	this->renderer = renderer;
	this->imagesFormat = mainWindow->getSurfaceFormat().format;

	setupSwapExtent();

	initSwapchain();
	initSwapchainImgs();
	initDepthStencilImage();
}

Swapchain::Swapchain()
{
}


Swapchain::~Swapchain()
{
	cleanup();
}

void Swapchain::setupSwapExtent() {
	VkSurfaceCapabilitiesKHR capabilities = mainWindow->getSurfaceCapatibilities();			//Ovo ce nam reci koja je maksimalna rezolucija slike za povrsinu
	VkExtent2D actualSizes = mainWindow->getSurfaceSize();
	this->isWSISupported = mainWindow->getIsWSISupported();

	if (capabilities.currentExtent.width != UINT32_MAX) {									//Ako je true, automatski se bira rezolucija najbolja za povrsinu
		this->swapExtent = capabilities.currentExtent;
	}
	else {
		actualSizes.width = std::clamp(actualSizes.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualSizes.height = std::clamp(actualSizes.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		this->swapExtent = actualSizes;
	}

	this->sizeX = swapExtent.width;
	this->sizeY = swapExtent.height;

	this->swapchainImageCount = capabilities.minImageCount + 1;								//Koliko slika ce biti u redu swapchaina?
	if (capabilities.maxImageCount > 0 && this->swapchainImageCount > capabilities.maxImageCount) {
		this->swapchainImageCount = capabilities.maxImageCount;
	}
}


void Swapchain::initSwapchain() {
	VkPresentModeKHR presentMode = getAvaiablePresentMode();
	QueueFamilyIndices indices = *mainWindow->getRenderer()->getQueueIndices();

	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = this->mainWindow->getSurface();
	swapchainCreateInfo.minImageCount = swapchainImageCount;									//Bufferovanje slika display buffera, koliko slika odjednom moze biti u redu
	swapchainCreateInfo.imageFormat = this->mainWindow->getSurfaceFormat().format;
	swapchainCreateInfo.imageColorSpace = this->mainWindow->getSurfaceFormat().colorSpace;
	swapchainCreateInfo.imageExtent = this->swapExtent;
	swapchainCreateInfo.imageArrayLayers = 1;													//Koliko slojeva ima slika (1 je obicno renderovanje, 2 je stetoskopsko)
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;						//Za koju vrstu operacija koristimo slike? Renderujemo ih, sto znaci da su oni COLOR ATTACHMENTS						
	if (indices.getGraphicsFamilyIndex() != indices.getPresentationFamilyIndex()) {
		uint32_t queueIndices[] = { indices.getGraphicsFamilyIndex(), indices.getPresentationFamilyIndex() };
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;						//Slika moze da se koristi paralelno, bez transfera vlasnistva nad slikom.
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices = queueIndices;
	}
	else {
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;						//Slika je u vlasnistvu jednog reda u jedno vreme, i vlasnistvo mora biti prebaceno na drugi da bi taj drugi mogao da ga koristi.
		swapchainCreateInfo.queueFamilyIndexCount = 0;											//Za exclusive je uvek 0
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;										//Ignorisemo za Exclusive
	}

	swapchainCreateInfo.preTransform = mainWindow->getSurfaceCapatibilities().currentTransform;	//mainWindow->getCapabilities().currentTransform ako necemo transformaciju. VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;						//Alfa kanal SURFACE-a, da li je ona transparentna
	swapchainCreateInfo.presentMode = presentMode;												//Vertical Sync
	swapchainCreateInfo.clipped = VK_TRUE;														//Ukljucujemo clipping, jako bitno za telefone
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;											//Ako rekonstruisemo swapchain, pokazivac na stari

	util->ErrorCheck(vkCreateSwapchainKHR(renderer->getDevice(), &swapchainCreateInfo, nullptr, &swapchain));
	util->ErrorCheck(vkGetSwapchainImagesKHR(renderer->getDevice(), swapchain, &swapchainImageCount, nullptr));
}

void Swapchain::initSwapchainImgs()
{
	images.resize(swapchainImageCount);
	imageViews.resize(swapchainImageCount);

	util->ErrorCheck(vkGetSwapchainImagesKHR(renderer->getDevice(), swapchain, &swapchainImageCount, images.data()));

	for (uint32_t i = 0; i < swapchainImageCount; i++) {
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
		imgCreateInfo.format = this->imagesFormat;
		imgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imgCreateInfo.image = images[i];
		imgCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;


		util->ErrorCheck(vkCreateImageView(renderer->getDevice(), &imgCreateInfo, nullptr, &imageViews[i]));
	}
}

void Swapchain::initDepthStencilImage()
{
	if(isStencilAvaiable()) {
		VkImageCreateInfo imgInfo = {};
		imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imgInfo.flags = 0;												//Pogledaj sta ima i dokumentaciju
		imgInfo.imageType = VK_IMAGE_TYPE_2D;
		imgInfo.extent.width = sizeX;
		imgInfo.extent.height = sizeY;
		imgInfo.extent.depth = 1;
		imgInfo.mipLevels = 1;											//Nivo mipmapinga. Ako je 0 nema slike.
		imgInfo.arrayLayers = 1;										//Slojevi slike. Ako je 0 nema slike.
		imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;						//Multisampling, ne koristimo multisample. Ako koristimo multisample, moramo da koristimo isti sample i za depthStencil i sa swapchain slike
		imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;						//Kako se slika dobija, ovo je bitno za teksture. (Fragmentacija trouglova)
		imgInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;	//Kako koristimo sliku
		imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;				//Da li delimo sliku izmedju redova (Trenutno ne)
		imgInfo.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
		imgInfo.pQueueFamilyIndices = nullptr;
		imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;				//Slike u Vulkanu imaju uvek neki layout.
		imgInfo.format = depthStencilFormat;							//Moramo da proverimo koji format je podrzan na nasoj GPU

		VkMemoryRequirements memoryRequirements = {};

		vkCreateImage(renderer->getDevice(), &imgInfo, nullptr, &depthStencilImage);		//Ako izostavimo metodu ispod, nece nam raditi program jer nije alocirana memorija za sliku
		vkGetImageMemoryRequirements(renderer->getDevice(), this->depthStencilImage, &memoryRequirements);


		VkPhysicalDeviceMemoryProperties memoryProps = renderer->getPhysicalDeviceMemoryProperties();

		uint32_t memoryIndex = this->util->findMemoryTypeIndex(&memoryProps, &memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		this->allocateInfo.allocationSize = memoryRequirements.size;
		this->allocateInfo.memoryTypeIndex = memoryIndex;
		this->allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

		//Postoji ogranicen broj alokacija na GPU-u.
		vkAllocateMemory(renderer->getDevice(), &this->allocateInfo, nullptr, &this->depthStencilImageMemory);
		vkBindImageMemory(renderer->getDevice(), depthStencilImage, depthStencilImageMemory, 0);

		VkImageViewCreateInfo imgCreateInfo = {};
		imgCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imgCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imgCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imgCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imgCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | (stencilAvaiable ? VK_IMAGE_ASPECT_STENCIL_BIT : 0);
		imgCreateInfo.subresourceRange.baseMipLevel = 0;
		imgCreateInfo.subresourceRange.levelCount = 1;
		imgCreateInfo.subresourceRange.baseArrayLayer = 0;
		imgCreateInfo.subresourceRange.layerCount = 1;
		imgCreateInfo.format = depthStencilFormat;
		imgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imgCreateInfo.image = depthStencilImage;
		imgCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

		vkCreateImageView(renderer->getDevice(), &imgCreateInfo, nullptr, &depthStencilImageView);
	}
}

VkPresentModeKHR Swapchain::getAvaiablePresentMode() {
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;	//Neki drajveri ne podrzavaju Mailbox. FIFO je zagarantovano podrzan kako pise u specifikaciji Vulkan API-a. 

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->getGpu(), this->mainWindow->getSurface(), &presentModeCount, nullptr);
	std::vector<VkPresentModeKHR> presentModeList(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->getGpu(), this->mainWindow->getSurface(), &presentModeCount, presentModeList.data());

	if (std::find(presentModeList.begin(), presentModeList.end(), VK_PRESENT_MODE_MAILBOX_KHR) != presentModeList.end()) {
		presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	}

	return presentMode;
}

bool Swapchain::isStencilAvaiable() {
	std::vector<VkFormat> tryoutFormats {
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D32_SFLOAT
	};

	for (auto f : tryoutFormats) {
		VkFormatProperties formatProperties = {};
		vkGetPhysicalDeviceFormatProperties(renderer->getGpu(), f, &formatProperties);

		if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			depthStencilFormat = f;
			break;
		}
	}

	if (depthStencilFormat == VK_FORMAT_UNDEFINED) {
		assert(0 && "Vulkan Error: No depth stencil format avaiable");
		exit(-1);
		return false;
	}

	stencilAvaiable = depthStencilFormat == VK_FORMAT_D32_SFLOAT_S8_UINT ||
		depthStencilFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
		depthStencilFormat == VK_FORMAT_D16_UNORM_S8_UINT ||
		depthStencilFormat == VK_FORMAT_D32_SFLOAT;

	return stencilAvaiable;
}

VkSwapchainKHR Swapchain::getSwapchain()
{
	return this->swapchain;
}

VkSwapchainKHR * Swapchain::getSwapchainPTR()
{
	return &swapchain;
}

uint32_t* Swapchain::getActiveImageSwapchainPTR()
{
	return &activeImageSwapchainId;
}

uint32_t Swapchain::getActiveImageSwapchain()
{
	return this->activeImageSwapchainId;
}

VkFormat Swapchain::getDepthStencilFormat()
{
	return this->depthStencilFormat;
}

uint32_t Swapchain::getSwapchainImageCount()
{
	return this->swapchainImageCount;
}

VkImageView Swapchain::getDepthStencilImageView()
{
	return this->depthStencilImageView;
}

std::vector<VkImageView> Swapchain::getImageViews()
{
	return this->imageViews;
}

void Swapchain::cleanup()
{
	for (VkImageView view : imageViews) {
		vkDestroyImageView(renderer->getDevice(), view, nullptr);
	}

	for (VkImage image : images) {
		vkDestroyImage(renderer->getDevice(), image, nullptr);
	}

	vkDestroyImageView(renderer->getDevice(), depthStencilImageView, nullptr);
	vkFreeMemory(renderer->getDevice(), this->depthStencilImageMemory, nullptr);
	vkDestroyImage(renderer->getDevice(), depthStencilImage, nullptr);

	vkDestroySwapchainKHR(renderer->getDevice(), swapchain, nullptr);
}