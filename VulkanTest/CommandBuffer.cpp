#include "pch.h"
#include "CommandBuffer.h"


CommandBuffer::CommandBuffer(VkCommandPool commandPool)
{
	Util& util = Util::instance();

	this->allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	this->allocateInfo.commandPool = commandPool;
	this->allocateInfo.commandBufferCount = 1;
	this->allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	
	this->beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//	this->beginInfo.flags = PEEK DEFINITION

	vkAllocateCommandBuffers(util.getDevice(), &this->allocateInfo, &this->commandBuffer);
}

void CommandBuffer::doSomeWork(VkQueue queue) {
	vkBeginCommandBuffer(this->commandBuffer, &this->beginInfo);

	VkViewport viewport = {};
	viewport.height = 320;
	viewport.width = 640;
	viewport.maxDepth = 1.0f;
	viewport.minDepth = 0.0f;
	viewport.x = 0;
	viewport.y = 0;

	vkCmdSetViewport(this->commandBuffer, 0, 0, &viewport);

	vkEndCommandBuffer(this->commandBuffer);						//Pretvara command buffer u executable koji GPU izvrsava1
}

/*VkQueue - U koji red bi trebalo da se submituje posao bafera.
  VkPipelineStageFlags - U kom trenutku u pipeline-u Vulkan Core-a bi semafori trebalo da reaguju na ovaj submit
  ComandBufferSemaphoreInfo - informacije koje su potrebne za reagovanje nad semaforima*/
bool CommandBuffer::submitQueue(VkQueue queue, VkPipelineStageFlags* flags, CommandBufferSemaphoreInfo semaphoreInfo){
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &this->commandBuffer;

	if (&semaphoreInfo != NULL) {
		VkSemaphore sem = semaphoreInfo.getSemaphore();
		
		if (&semaphoreInfo != NULL && semaphoreInfo.getShouldWaitForSignalization()) {
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &sem;
		}
		else if (&semaphoreInfo != NULL) {
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &sem;
			submitInfo.pWaitDstStageMask = semaphoreInfo.getPipelineStages();
		}
	}

	return VK_SUCCESS == vkQueueSubmit(queue, 1, &submitInfo, this->fence);
}

void CommandBuffer::createFence() {
	Util& util = Util::instance();

	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	vkCreateFence(util.getDevice(), &fenceCreateInfo, nullptr, &fence);
}

VkFence CommandBuffer::getFence() {
	return this->fence;
}

VkCommandBuffer CommandBuffer::getCommandBuffer() {
	return this->commandBuffer;
}

CommandBuffer::~CommandBuffer()
{
	Util& util = Util::instance();
	vkDestroyFence(util.getDevice(), this->fence, nullptr);
}
