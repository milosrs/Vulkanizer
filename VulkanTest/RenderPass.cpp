#include "pch.h"
#include "RenderPass.h"


RenderPass::RenderPass(Renderer* renderer, VkFormat depthStencilFormat, VkSurfaceFormatKHR surfaceFormat)
{
	util = &Util::instance();
	this->renderer = renderer;
	
	initRenderPass(depthStencilFormat, surfaceFormat);
}

RenderPass::RenderPass()
{
}


RenderPass::~RenderPass()
{
	destroyRenderPass();
}

VkRenderPass RenderPass::getRenderPass()
{
	return this->renderPass;
}

void RenderPass::initRenderPass(VkFormat depthStencilFormat, VkSurfaceFormatKHR surfaceFormat)
{
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

	subpasses[0].colorAttachmentCount = subpassAttachments.size();
	subpasses[0].pColorAttachments = subpassAttachments.data();
	subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[0].inputAttachmentCount = 0;
	subpasses[0].pInputAttachments = nullptr;
	subpasses[0].pDepthStencilAttachment = &depthStencilAttachment;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = subpasses.size();
	renderPassInfo.pSubpasses = subpasses.data();


	util->ErrorCheck(vkCreateRenderPass(renderer->getDevice(), &renderPassInfo, nullptr, &renderPass));
}

void RenderPass::destroyRenderPass()
{
	vkDestroyRenderPass(renderer->getDevice(), renderPass, nullptr);
}