#include "pch.h"
#include "RenderPass.h"
#include "DepthTester.h"

RenderPass::RenderPass(Renderer* renderer, VkSurfaceFormatKHR surfaceFormat)
{
	this->renderer = renderer;
	this->surfaceFormat = surfaceFormat;
	
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
	std::array<VkAttachmentDescription, 2> attachments;
	std::array<VkAttachmentReference, 2> attachmentRefs;
	VkSubpassDescription subpass = {};
	VkRenderPassCreateInfo info = {};

	attachments[0].format = DepthTester::findFormat(renderer->getDevice(), renderer->getGpu());
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachments[0].flags = 0;

	attachments[1].format = surfaceFormat.format;						//Mora da se poklapa sa formatom slika iz swapchaina
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;						//Odnosi se na multisampling
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;				//Operacija koju render pass attachment treba da obavi pri ucitavanju
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;				//Operacija koju treba odraditi posle rendera
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;			//Nije nam bitno kog je formata bila prosla slika, to ovo znaci.
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;		//Slike koje treba da budu predstavljene u swapchainu
	attachments[1].flags = 0;

	attachmentRefs[0].attachment = 1;
	attachmentRefs[0].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	attachmentRefs[1].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;//Layout slike u ovom subpassu
	attachmentRefs[1].attachment = 0;									//Index attachmenta koji referenciramo ovim subpassom

	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pDepthStencilAttachment = &attachmentRefs[0];
	subpass.pColorAttachments = &attachmentRefs[1];

	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstSubpass = 0;
	subpassDependency.dstAccessMask = 0;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	info.attachmentCount = attachments.size();
	info.dependencyCount = 1;
	info.pAttachments = attachments.data();
	info.pDependencies = &subpassDependency;
	info.subpassCount = 1;
	info.pSubpasses = &subpass;
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	Util::ErrorCheck(vkCreateRenderPass(this->renderer->getDevice(), &info, nullptr, &this->renderPass));
}

VkRenderPass RenderPass::getRenderPass()
{
	return this->renderPass;
}

VkRenderPass * RenderPass::getRenderPassPTR()
{
	return &this->renderPass;
}
