#include "pch.h"
#include "CommandBuffer.h"


CommandBuffer::CommandBuffer(VkCommandPool commandPool, VkDevice device)
{
	util = &Util::instance();

	this->device = device;
	this->allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	this->allocateInfo.commandPool = commandPool;
	this->allocateInfo.commandBufferCount = 1;
	this->allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;				//Moze da bude submitovan u Queue, ostali moraju da budu pozivani iz primarnog.
	
	this->beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	this->beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;	//Submitovacemo ovaj command buffer samo jednom u red. Dovoljno da se izrenderuje.
	//this->beginInfo.pInheritanceInfo = KORISTI SE ZA SEKUNDARNE I OSTALE REDOVE 
}

void CommandBuffer::startRecording() {
	Util& util = Util::instance();
	vkAllocateCommandBuffers(device, &this->allocateInfo, &this->commandBuffer);
}

void CommandBuffer::endRecording() {
	vkEndCommandBuffer(this->commandBuffer);
}

void CommandBuffer::doSomeWork(VkQueue queue, VkViewport* viewport) {
	util->ErrorCheck(vkBeginCommandBuffer(this->commandBuffer, &this->beginInfo));

	vkCmdSetViewport(this->commandBuffer, 0, 0, viewport);

	util->ErrorCheck(vkEndCommandBuffer(this->commandBuffer));						//Pretvara command buffer u executable koji GPU izvrsava1
}

/*VkQueue - U koji red bi trebalo da se submituje posao bafera.
  VkPipelineStageFlags - U kom trenutku u pipeline-u Vulkan Core-a bi semafori trebalo da reaguju na ovaj submit
  ComandBufferSemaphoreInfo - informacije koje su potrebne za reagovanje nad semaforima*/
bool CommandBuffer::submitQueue(VkQueue queue, VkPipelineStageFlags* flags, CommandBufferSemaphoreInfo* semaphoreInfo){
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &this->commandBuffer;

	if (&semaphoreInfo != NULL) {
		VkSemaphore sem = semaphoreInfo->getSemaphore();
		
		if (&semaphoreInfo != NULL && semaphoreInfo->getShouldWaitForSignalization()) {
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &sem;
		}
		else if (&semaphoreInfo != NULL) {
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &sem;
			submitInfo.pWaitDstStageMask = semaphoreInfo->getPipelineStages();
		}
	}

	return VK_SUCCESS == vkQueueSubmit(queue, 1, &submitInfo, this->fence);
}

void CommandBuffer::createFence() {
	Util& util = Util::instance();

	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);
}

VkFence CommandBuffer::getFence() {
	return this->fence;
}

VkCommandBuffer CommandBuffer::getCommandBuffer() {
	return this->commandBuffer;
}

CommandBuffer::~CommandBuffer()
{
	vkDestroyFence(device, this->fence, nullptr);
}
