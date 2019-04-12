#include "pch.h"
#include "Swapchain.h"
#include "MainWindow.h"
#include "Renderer.h"
#include "WindowController.h"
#include "QueueFamilyIndices.h"
#include "CommandBufferHandler.h"
#include "Util.h"

Swapchain::Swapchain()
{
	mainWindow = &MainWindow::getInstance();
	renderer = mainWindow->getRenderer();

	imagesFormat = mainWindow->getSurfaceFormat().format;
	setupSwapExtent();

	initSwapchain();
	initSwapchainImgs();
	setupScreenshotData();
}

Swapchain::~Swapchain()
{
	cleanup();
}

void Swapchain::setupSwapExtent() {
	MainWindow *mainWindow = &MainWindow::getInstance();

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
	QueueFamilyIndices *indices = mainWindow->getRenderer()->getQueueIndices();

	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = this->mainWindow->getSurface();
	swapchainCreateInfo.minImageCount = swapchainImageCount;									//Bufferovanje slika display buffera, koliko slika odjednom moze biti u redu
	swapchainCreateInfo.imageFormat = this->mainWindow->getSurfaceFormat().format;
	swapchainCreateInfo.imageColorSpace = this->mainWindow->getSurfaceFormat().colorSpace;
	swapchainCreateInfo.imageExtent = this->swapExtent;
	swapchainCreateInfo.imageArrayLayers = 1;													//Koliko slojeva ima slika (1 je obicno renderovanje, 2 je stetoskopsko)
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;						//Za koju vrstu operacija koristimo slike? Renderujemo ih, sto znaci da su oni COLOR ATTACHMENTS						
	if (indices->getGraphicsFamilyIndex() != indices->getPresentationFamilyIndex()) {
		uint32_t queueIndices[] = { indices->getGraphicsFamilyIndex(), indices->getPresentationFamilyIndex() };
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;						//Slika moze da se koristi paralelno, bez transfera vlasnistva nad slikom.
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices = queueIndices;
	}
	else {
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;						//Slika je u vlasnistvu jednog reda u jedno vreme, i vlasnistvo mora biti prebaceno na drugi da bi taj drugi mogao da ga koristi.
		swapchainCreateInfo.queueFamilyIndexCount = 0;											//Za exclusive je uvek 0
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;										//Ignorisemo za Exclusive
	}

	if (mainWindow->getSurfaceCapatibilities().supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
		swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}
	if (mainWindow->getSurfaceCapatibilities().supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
		swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	swapchainCreateInfo.preTransform = mainWindow->getSurfaceCapatibilities().currentTransform;	//mainWindow->getCapabilities().currentTransform ako necemo transformaciju. VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;						//Alfa kanal SURFACE-a, da li je ona transparentna
	swapchainCreateInfo.presentMode = presentMode;												//Vertical Sync
	swapchainCreateInfo.clipped = VK_TRUE;														//Ukljucujemo clipping, jako bitno za telefone
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;											//Ako rekonstruisemo swapchain, pokazivac na stari

	Util::ErrorCheck(vkCreateSwapchainKHR(renderer->getDevice(), &swapchainCreateInfo, nullptr, &swapchain));
}

void Swapchain::initSwapchainImgs()
{
	Util::ErrorCheck(vkGetSwapchainImagesKHR(renderer->getDevice(), swapchain, &swapchainImageCount, nullptr));
	images.resize(swapchainImageCount);
	imageViews.resize(swapchainImageCount);
	Util::ErrorCheck(vkGetSwapchainImagesKHR(renderer->getDevice(), swapchain, &swapchainImageCount, images.data()));

	for (uint32_t i = 0; i < swapchainImageCount; i++) {
		imageViews[i] = Util::createImageView(this->mainWindow->getRenderer()->getDevice(),
			images[i], imagesFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

void Swapchain::setupScreenshotData()
{
	VkFormatProperties formatProps;
	supportsBlit = true;

	//Does device support blitting from optimal tiled images?
	vkGetPhysicalDeviceFormatProperties(mainWindow->getRenderer()->getGpu(), this->imagesFormat, &formatProps);
	if (!(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT)) {
		assert(0 && "Screenshot error: Device does not support blitting from optimal tiled images.");
		supportsBlit = false;
		return;
	}

	//Does device support blitting from linear tiled images?
	vkGetPhysicalDeviceFormatProperties(mainWindow->getRenderer()->getGpu(), VK_FORMAT_R8G8B8A8_UNORM, &formatProps);
	if (!(formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT)) {
		assert(0 && "Screenshot error: Device does not support blitting from linear tiled images.");
		supportsBlit = false;
		return;
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

bool Swapchain::isSupportingBlit()
{
	return supportsBlit;
}

void Swapchain::saveScreenshot(std::string filePath)
{
	VkImageCreateInfo screenshotCreateInfo;
	VkImage srcImage = images[activeImageSwapchainId];
	VkDeviceMemory dstImageMemory;
	VkImage dstImage = VK_NULL_HANDLE;
	
	if (supportsBlit) {
		VkCommandBuffer cmdBuffer = CommandBufferHandler::createOneTimeUsageBuffer(
			mainWindow->getCommandHandler()->getCommandPool(), mainWindow->getRenderer()->getDevice());

		Util::createImage(sizeX, sizeY, 1, VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			&dstImage, &dstImageMemory, mainWindow->getRenderer()->getDevice(),
			mainWindow->getRenderer()->getPhysicalDeviceMemoryPropertiesPTR());

		//NOVU Sliku prebacujemo u transfer destination
		Util::transitionImageLayout(dstImage,
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			mainWindow->getCommandHandler()->getCommandPool(),
			mainWindow->getRenderer()->getQueueIndices()->getQueue(),
			mainWindow->getRenderer()->getDevice(),
			1,
			cmdBuffer);

		//Staru sliku prebacujemo iz prezentacije u transfer source
		Util::transitionImageLayout(srcImage,
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			mainWindow->getCommandHandler()->getCommandPool(),
			mainWindow->getRenderer()->getQueueIndices()->getQueue(),
			mainWindow->getRenderer()->getDevice(),
			1,
			cmdBuffer);

		VkExtent3D extent = { swapExtent.width, swapExtent.height, 1 };
		VkOffset3D offsets{};
		offsets.x = 0;
		offsets.y = 0;
		offsets.z = 0;

		VkImageSubresourceLayers layers{};
		layers.mipLevel = 0;
		layers.layerCount = 1;
		layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		layers.baseArrayLayer = 0;
		
		VkImageCopy	regions{};
		regions.dstOffset = offsets;
		regions.extent = extent;
		regions.srcOffset = offsets;
		regions.srcSubresource = layers;
		regions.dstSubresource = layers;

		vkCmdCopyImage(cmdBuffer,
			srcImage,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			dstImage,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&regions);

		Util::transitionImageLayout(dstImage, VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL,
			mainWindow->getCommandHandler()->getCommandPool(),
			mainWindow->getRenderer()->getQueueIndices()->getQueue(),
			mainWindow->getRenderer()->getDevice(),
			1,
			cmdBuffer);

		Util::transitionImageLayout(srcImage, VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			mainWindow->getCommandHandler()->getCommandPool(),
			mainWindow->getRenderer()->getQueueIndices()->getQueue(),
			mainWindow->getRenderer()->getDevice(),
			1,
			cmdBuffer);

		CommandBufferHandler::endOneTimeUsageBuffer(cmdBuffer, mainWindow->getRenderer()->getQueueIndices()->getQueue(),
			mainWindow->getCommandHandler()->getCommandPool(), mainWindow->getRenderer()->getDevice());

		unsigned char *data = nullptr;
		vkMapMemory(mainWindow->getRenderer()->getDevice(), dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&data);
		
		MagickCore::MagickWand *wand = MagickCore::NewMagickWand();
		MagickConstituteImage(wand, swapExtent.width, swapExtent.height, "BGRA", MagickCore::CharPixel, data);

		vkUnmapMemory(mainWindow->getRenderer()->getDevice(), dstImageMemory);
		vkFreeMemory(mainWindow->getRenderer()->getDevice(), dstImageMemory, nullptr);
		vkDestroyImage(mainWindow->getRenderer()->getDevice(), dstImage, nullptr);

		MagickCore::MagickBooleanType status = MagickWriteImage(wand, filePath.c_str());

		if (!status) {
			assert(0 && "ImageMagick error: Unable to save image to file.");
		}

		MagickCore::DestroyMagickWand(wand);
		MagickCore::MagickWandTerminus();
	}
	else {
		assert(0 && "Swapchain error: Your device does not support image blitting. Cannot save image. Will be patched.");
		return;
	}
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