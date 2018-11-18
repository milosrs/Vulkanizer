#include "pch.h"
#include "RenderPass.h"


RenderPass::RenderPass(Renderer* renderer, VkFormat depthStencilFormat, VkSurfaceFormatKHR surfaceFormat)
{
	util = &Util::instance();
	this->renderer = renderer;
	this->depthStencilFormat = depthStencilFormat;
	this->surfaceFormat = surfaceFormat;
	
	initRenderPass();
}

RenderPass::RenderPass()
{
}


RenderPass::~RenderPass()
{
	vkDestroyRenderPass(renderer->getDevice(), renderPass, nullptr);
}

void RenderPass::initRenderPass()
{
	//createDepthStencil();
	createColor();
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

	info.attachmentCount = 1;
	info.dependencyCount = 0;
	info.pAttachments = &attachment;
	info.pDependencies = nullptr;
	info.subpassCount = 1;
	info.pSubpasses = &subpass;
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	util->ErrorCheck(vkCreateRenderPass(this->renderer->getDevice(), &info, nullptr, &this->renderPass));
}

void RenderPass::beginRenderPass(VkFramebuffer frameBuffer, VkExtent2D extent, VkCommandBuffer commandBuffer)
{
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = frameBuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = extent;

	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };		//Odnosi se na LOAD_OP_CLEAR 
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::endRenderPass(VkCommandBuffer commandBuffer)
{
	vkCmdEndRenderPass(commandBuffer);
}

void RenderPass::createDepthStencil() {
	std::array<VkAttachmentDescription, 2> attachments{};
	std::array<VkSubpassDescription, 1> subpasses{};
	std::array<VkAttachmentReference, 1> subpassAttachments{};
	VkAttachmentReference depthStencilAttachment;

	attachments[0].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachments[0].format = depthStencilFormat;
	attachments[0].flags = 0;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	attachments[1].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachments[1].format = surfaceFormat.format;
	attachments[1].flags = 0;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	depthStencilAttachment.attachment = 0;
	depthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	subpassAttachments[0].attachment = 1;
	subpassAttachments[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	subpasses[0].colorAttachmentCount = static_cast<uint32_t>(subpassAttachments.size());
	subpasses[0].pColorAttachments = subpassAttachments.data();
	subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[0].inputAttachmentCount = 0;
	subpasses[0].pInputAttachments = nullptr;
	subpasses[0].pDepthStencilAttachment = &depthStencilAttachment;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
	renderPassInfo.pSubpasses = subpasses.data();

	util->ErrorCheck(vkCreateRenderPass(renderer->getDevice(), &renderPassInfo, nullptr, &renderPass));
}

VkRenderPass RenderPass::getRenderPass()
{
	return this->renderPass;
}

VkRenderPass * RenderPass::getRenderPassPTR()
{
	return &this->renderPass;
}