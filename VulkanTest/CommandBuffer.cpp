#include "pch.h"
#include "CommandBuffer.h"



CommandBuffer::CommandBuffer(VkCommandPool commandPool, VkDevice device, VkCommandBufferUsageFlags useFlags, CommandBufferType type)
{
	util = &Util::instance();

	this->device = device;
	this->commandPool = commandPool;
	this->type = type;

	this->allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	this->allocateInfo.commandPool = commandPool;
	this->allocateInfo.commandBufferCount = 1;
	this->allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;				//Moze da bude submitovan u Queue, ostali moraju da budu pozivani iz primarnog.
	
	this->beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	this->beginInfo.flags = useFlags;	//Submitovacemo ovaj command buffer samo jednom u red. Dovoljno da se izrenderuje.
	//this->beginInfo.pInheritanceInfo = KORISTI SE ZA SEKUNDARNE I OSTALE REDOVE 
}

CommandBuffer::CommandBuffer(const CommandBuffer &)
{
}

void CommandBuffer::allocateCommandBuffer() {
	vkAllocateCommandBuffers(device, &this->allocateInfo, &this->commandBuffer);
}

void CommandBuffer::endCommandBuffer() {
	vkEndCommandBuffer(this->commandBuffer);//Pretvara command buffer u executable koji GPU izvrsava1
}

void CommandBuffer::startCommandBuffer(VkViewport* viewport) {
	util->ErrorCheck(vkBeginCommandBuffer(this->commandBuffer, &this->beginInfo));
	
	if (viewport != nullptr) {
		vkCmdSetViewport(this->commandBuffer, 0, 1, viewport);
	}
}

void CommandBuffer::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size, VkQueue queue)
{
	allocateCommandBuffer();
	startCommandBuffer();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;

	vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);
	endCommandBuffer();

	submitQueue(device, queue, nullptr, nullptr, nullptr);
	vkQueueWaitIdle(queue);
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

bool CommandBuffer::submitQueue(VkDevice device, VkQueue queue, 
								CommandBufferSemaphoreInfo* waitSemaphoreInfo, 
								CommandBufferSemaphoreInfo* signalSemaphoreInfo,  
								VkFence* fence){
	VkSubmitInfo submitInfo = {};
	VkSemaphore* waitSemaphore;
	VkSemaphore* signalSemaphore;
	bool success = true;
	VkResult result;

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &this->commandBuffer;

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
		vkResetFences(device, 1, fence);
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

VkCommandBuffer CommandBuffer::getCommandBuffer() {
	return this->commandBuffer;
}

CommandBufferType CommandBuffer::getType()
{
	return this->type;
}

CommandBuffer::~CommandBuffer()
{
}
