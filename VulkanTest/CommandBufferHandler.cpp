#include "pch.h"
#include "CommandBufferHandler.h"
#include "MainWindow.h"

CommandBufferHandler::CommandBufferHandler(uint32_t graphicsFamilyIndex, VkDevice device, MainWindow* window)
{
	VkCommandPoolCreateInfo cmdPoolCreateInfo{};
	VkCommandPoolCreateInfo transferCmdPoolCreateInfo{};

	this->util = &Util::instance();
	this->device = device;
	this->window = window;

	cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreateInfo.queueFamilyIndex = graphicsFamilyIndex;

	transferCmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	transferCmdPoolCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
	transferCmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

	vkCreateCommandPool(device, &cmdPoolCreateInfo, nullptr, &cmdPool);
	vkCreateCommandPool(device, &transferCmdPoolCreateInfo, nullptr, &transferCommandPool);

	renderArea.offset.x = 0;
	renderArea.offset.y = 0;
	renderArea.extent = window->getSurfaceSize();
}


CommandBufferHandler::~CommandBufferHandler()
{
	vkDestroyCommandPool(this->device, this->cmdPool, nullptr);
	this->cmdPool = nullptr;
}

void CommandBufferHandler::createDrawingCommandBuffers(uint32_t bufferCount)
{
	this->drawingBuffersCount = bufferCount;

	VkCommandBufferAllocateInfo allocateInfo = {};
	VkCommandBufferBeginInfo beginInfo = {};
	std::vector<VkCommandBuffer> cmdBuffers;

	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = cmdPool;
	allocateInfo.commandBufferCount = bufferCount;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;				//Moze da bude submitovan u Queue, ostali moraju da budu pozivani iz primarnog.

	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	cmdBuffers.resize(bufferCount);

	vkAllocateCommandBuffers(device, &allocateInfo, cmdBuffers.data());

	for (auto i = 0; i < cmdBuffers.size(); ++i) {
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = window->getRenderPass()->getRenderPass();
		renderPassBeginInfo.framebuffer = window->getActiveFrameBuffer()->getFrameBuffers()[i];
		renderPassBeginInfo.renderArea = renderArea;
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearValues;

		CommandBuffer cmdBuffer = {};
		cmdBuffer.type = CommandBufferType::GRAPHICS;
		cmdBuffer.commandBuffer = cmdBuffers[i];

		this->commandBuffers.push_back(cmdBuffer);

		util->ErrorCheck(vkBeginCommandBuffer(cmdBuffer.commandBuffer, &beginInfo));

		window->getRenderPass()->beginRenderPass(cmdBuffer.commandBuffer, &renderPassBeginInfo);
		window->getPipelinePTR()->bindPipeline(cmdBuffer.commandBuffer,
			window->getVertexBufferPTR(), window->getIndexBufferPTR());
		window->getDescriptorHandler()->bind(cmdBuffer.commandBuffer, window->getPipelinePTR()->getPipelineLayout(), i);

		vkCmdDrawIndexed(cmdBuffer.commandBuffer, static_cast<uint32_t>(window->getVertexBufferPTR()->getIndices().size()), 
			1, 0, 0, 0);

		window->getRenderPass()->endRenderPass(cmdBuffer.commandBuffer);
		util->ErrorCheck(vkEndCommandBuffer(cmdBuffer.commandBuffer));//Pretvara command buffer u executable koji GPU izvrsava
	}
}

void CommandBufferHandler::createTransferCommandBuffers(uint32_t bufferCount) {
	VkCommandBufferAllocateInfo allocateInfo = {};
	
	std::vector<VkCommandBuffer> cmdBuffers;

	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = transferCommandPool;
	allocateInfo.commandBufferCount = bufferCount;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;				//Moze da bude submitovan u Queue, ostali moraju da budu pozivani iz primarnog.

	cmdBuffers.resize(bufferCount);

	vkAllocateCommandBuffers(device, &allocateInfo, cmdBuffers.data());

	for (auto i = 0; i < cmdBuffers.size(); ++i) {
		CommandBuffer cmdBuffer = {};
		cmdBuffer.type = CommandBufferType::TRANSFER;
		cmdBuffer.commandBuffer = cmdBuffers[i];
	}
}

void CommandBufferHandler::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size, VkQueue queue)
{
	VkCommandBufferBeginInfo beginInfo = {};
	CommandBuffer cmdBuff;
	uint32_t index = 0;

	for (CommandBuffer cmd : commandBuffers) {
		if (cmd.type == CommandBufferType::TRANSFER) {
			cmdBuff = cmd;
		}

		++index;
	}

	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdBuff.commandBuffer, &beginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;

	vkCmdCopyBuffer(cmdBuff.commandBuffer, src, dst, 1, &copyRegion);
	vkEndCommandBuffer(cmdBuff.commandBuffer);

	submitQueue(index, queue, nullptr, nullptr, nullptr);
	vkQueueWaitIdle(queue);
	vkFreeCommandBuffers(device, transferCommandPool, 1, &cmdBuff.commandBuffer);

	commandBuffers.erase(commandBuffers.begin() + index);
}

bool CommandBufferHandler::submitQueue(int commandBufferIndex, VkQueue queue, CommandBufferSemaphoreInfo *waitSemaphoreInfo,
										CommandBufferSemaphoreInfo *signalSemaphoreInfo, VkFence *fence)
{
	VkSubmitInfo submitInfo = {};
	VkSemaphore* waitSemaphore;
	VkSemaphore* signalSemaphore;
	bool success = true;
	VkResult result;

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[commandBufferIndex].commandBuffer;

	if (waitSemaphoreInfo != nullptr && signalSemaphoreInfo != nullptr) {
		waitSemaphore = waitSemaphoreInfo->getSemaphorePTR();
		signalSemaphore = signalSemaphoreInfo->getSemaphorePTR();

		if (waitSemaphore != nullptr) {
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphore;
			submitInfo.pWaitDstStageMask = waitSemaphoreInfo->getPipelineStages();
		}

		if (signalSemaphore != nullptr) {

			if (signalSemaphore != nullptr && signalSemaphoreInfo->getShouldWaitForSignalization()) {
				submitInfo.signalSemaphoreCount = 1;
				submitInfo.pSignalSemaphores = signalSemaphore;
			}
			else if (signalSemaphore != nullptr) {
				submitInfo.waitSemaphoreCount = 1;
				submitInfo.pSignalSemaphores = signalSemaphore;
				submitInfo.pWaitDstStageMask = signalSemaphoreInfo->getPipelineStages();
			}
		}
	}

	if (fence != nullptr) {
		result = vkQueueSubmit(queue, 1, &submitInfo, *fence);
	}
	else {
		result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	}

	success = result == VK_SUCCESS;

	if (!success) {
		util->ErrorCheck(result);
	}

	return success;
}

VkCommandBuffer CommandBufferHandler::createOneTimeUsageBuffer()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = cmdPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void CommandBufferHandler::endOneTimeUsageBuffer(VkCommandBuffer commandBuffer, VkQueue queue)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, cmdPool, 1, &commandBuffer);
}

std::vector<CommandBuffer> CommandBufferHandler::getCommandBuffers()
{
	return this->commandBuffers;
}

VkCommandPool CommandBufferHandler::getCommandPool() {
	return this->cmdPool;
}

