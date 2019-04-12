#include "pch.h"
#include "RenderPass.h"
#include "DepthTester.h"
#include "Renderer.h"
#include "Util.h"
#include "MainWindow.h"

RenderPass::RenderPass(VkSurfaceFormatKHR surfaceFormat, VkSampleCountFlagBits samples)
{
	MainWindow *mainWindow = &MainWindow::getInstance();
	this->renderer = mainWindow->getRenderer();
	this->surfaceFormat = surfaceFormat;
	this->samples = samples;
	
	initRenderPass();
}

RenderPass::~RenderPass()
{
	vkDestroyRenderPass(renderer->getDevice(), renderPass, nullptr);
}

void RenderPass::initRenderPass()
{
	if (DepthTester::isInstanceCreated()) {
		createColorDepth();
	}
	else {
		createColor();
	}
}

///Prvo pravimo subpass attachment, referenciramo attachment koristeci VkAttachmentReference, pa tek onda pravimo subpass.
void RenderPass::createColor() {
	VkAttachmentDescription attachment = {};
	VkAttachmentReference reference = {};
	VkSubpassDescription subpass = {};
	VkRenderPassCreateInfo info = {};

	attachment.format = surfaceFormat.format;					//Mora da se poklapa sa formatom slika iz swapchaina
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;					//Odnosi se na multisampling
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;			//Operacija koju render pass attachment treba da obavi pri ucitavanju
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;			//Operacija koju treba odraditi posle rendera
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;		//Nije nam bitno kog je formata bila prosla slika, to ovo znaci.
	attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;	//Slike koje treba da budu predstavljene u swapchainu

	reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;//Layout slike u ovom subpassu
	reference.attachment = 0;									//Index attachmenta koji referenciramo ovim subpassom

	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &reference;						//REFERENCIRAN JE IZ FRAGMENT SHADERA

	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstSubpass = 0;
	subpassDependency.dstAccessMask = 0;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	
	info.attachmentCount = 1;
	info.dependencyCount = 1;
	info.pAttachments = &attachment;
	info.pDependencies = &subpassDependency;
	info.subpassCount = 1;
	info.pSubpasses = &subpass;
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	Util::ErrorCheck(vkCreateRenderPass(this->renderer->getDevice(), &info, nullptr, &this->renderPass));
}

void RenderPass::beginRenderPass(VkCommandBuffer cmdBuffer, VkRenderPassBeginInfo* beginInfo) {
	vkCmdBeginRenderPass(cmdBuffer, beginInfo, VK_SUBPASS_CONTENTS_INLINE);		//Sadrzaj subpass-a ce biti snimljen u primarni cmd buffer. Sekundarni NE SMEJU da se pokrecu unutar ovog subpassa.
}

void RenderPass::endRenderPass(VkCommandBuffer commandBuffer)
{
	vkCmdEndRenderPass(commandBuffer);
}

void RenderPass::createColorDepth() 
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = surfaceFormat.format;
	colorAttachment.samples = samples;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachmentResolve = {};
	colorAttachmentResolve.format = surfaceFormat.format;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = DepthTester::findFormat(renderer->getDevice(), renderer->getGpu());
	depthAttachment.samples = samples;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentResolveRef = {};
	colorAttachmentResolveRef.attachment = 2;
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pResolveAttachments = &colorAttachmentResolveRef;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	Util::ErrorCheck(vkCreateRenderPass(renderer->getDevice(), &renderPassInfo, nullptr, &renderPass));
}

VkRenderPass RenderPass::getRenderPass()
{
	return this->renderPass;
}

VkRenderPass * RenderPass::getRenderPassPTR()
{
	return &this->renderPass;
}

VkSampleCountFlagBits RenderPass::getSamples()
{
	return samples;
}
