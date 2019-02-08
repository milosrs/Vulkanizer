#include "pch.h"
#include "CommandBufferHandler.h"
#include "MainWindow.h"
#include "RenderObject.h"

CommandBufferHandler::CommandBufferHandler(uint32_t graphicsFamilyIndex, VkDevice device)
{
	this->window = &MainWindow::getInstance();
	this->device = device;

	VkCommandPoolCreateInfo cmdPoolCreateInfo{};
	VkCommandPoolCreateInfo transferCmdPoolCreateInfo{};

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

void CommandBufferHandler::createDrawingCommandBuffers(uint32_t bufferCount, RenderObject *object)
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
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(object->getClearValues()->size());
		renderPassBeginInfo.pClearValues = object->getClearValues()->data();

		CommandBuffer cmdBuffer = {};
		cmdBuffer.type = CommandBufferType::GRAPHICS;
		cmdBuffer.commandBuffer = cmdBuffers[i];

		this->commandBuffers.push_back(cmdBuffer);

		Util::ErrorCheck(vkBeginCommandBuffer(cmdBuffer.commandBuffer, &beginInfo));

		window->getRenderPass()->beginRenderPass(cmdBuffer.commandBuffer, &renderPassBeginInfo);
		window->getPipelinePTR()->bindPipeline(cmdBuffer.commandBuffer,
			object->getVertexBuffer(), object->getIndexBuffer());
		object->getDescriptorHandler()->bind(cmdBuffer.commandBuffer, window->getPipelinePTR()->getPipelineLayout(), i);

		vkCmdDrawIndexed(cmdBuffer.commandBuffer, static_cast<uint32_t>(object->getVertices()->getIndices().size()),
			1, 0, 0, 0);

		window->getRenderPass()->endRenderPass(cmdBuffer.commandBuffer);
		Util::ErrorCheck(vkEndCommandBuffer(cmdBuffer.commandBuffer));	//Pretvara command buffer u executable koji GPU izvrsava
	}
}

void CommandBufferHandler::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size, VkQueue queue, VkCommandPool cmdPool, VkDevice device)
{
	VkCommandBuffer activeBuffer = createOneTimeUsageBuffer(cmdPool, device);
	
	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;

	vkCmdCopyBuffer(activeBuffer, src, dst, 1, &copyRegion);
	vkEndCommandBuffer(activeBuffer);
	
	endOneTimeUsageBuffer(activeBuffer, queue, cmdPool, device);
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
		Util::ErrorCheck(result);
	}

	return success;
}

VkCommandBuffer CommandBufferHandler::createOneTimeUsageBuffer(VkCommandPool cmdPool, VkDevice device)
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

void CommandBufferHandler::endOneTimeUsageBuffer(VkCommandBuffer commandBuffer, VkQueue queue, 
	VkCommandPool cmdPool, VkDevice device)
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

