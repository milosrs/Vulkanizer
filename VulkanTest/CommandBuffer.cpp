#include "pch.h"
#include "CommandBuffer.h"

CommandBuffer::CommandBuffer(VkCommandPool commandPool, VkDevice device)
{
	util = &Util::instance();

	this->device = device;
	this->commandPool = commandPool;

	this->allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	this->allocateInfo.commandPool = commandPool;
	this->allocateInfo.commandBufferCount = 1;
	this->allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;				//Moze da bude submitovan u Queue, ostali moraju da budu pozivani iz primarnog.
	
	this->beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	this->beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;	//Submitovacemo ovaj command buffer samo jednom u red. Dovoljno da se izrenderuje.
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
	
	vkCmdSetViewport(this->commandBuffer, 0, 1, viewport);
}

/*VkQueue - U koji red bi trebalo da se submituje posao bafera.
  VkPipelineStageFlags - U kom trenutku u pipeline-u Vulkan Core-a bi semafori trebalo da reaguju na ovaj submit
  ComandBufferSemaphoreInfo - informacije koje su potrebne za reagovanje nad semaforima*/
bool CommandBuffer::submitQueue(VkDevice device, VkQueue queue, CommandBufferSemaphoreInfo* waitSemaphoreInfo, CommandBufferSemaphoreInfo* signalSemaphoreInfo,  VkFence* fence){
	VkSubmitInfo submitInfo = {};
	VkSemaphore* waitSemaphore = waitSemaphoreInfo->getSemaphorePTR();
	VkSemaphore* signalSemaphore = signalSemaphoreInfo->getSemaphorePTR();

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &this->commandBuffer;

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

	vkResetFences(device, 1, fence);

	VkResult result = vkQueueSubmit(queue, 1, &submitInfo, *fence);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		return false;
	}

	if (result != VK_SUCCESS) {
		util->ErrorCheck(result);
		return false;
	}

	return true;
}

VkCommandBuffer CommandBuffer::getCommandBuffer() {
	return this->commandBuffer;
}

CommandBuffer::~CommandBuffer()
{
}
