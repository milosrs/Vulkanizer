#include "pch.h"
#include "CommandBufferSemaphoreInfo.h"

CommandBufferSemaphoreInfo::CommandBufferSemaphoreInfo(bool shouldWaitForSignalization, VkSemaphore semaphore, VkPipelineStageFlags* pipelineStages) {
	this->shouldWaitForSignalization = shouldWaitForSignalization;
	this->semaphore = semaphore;
	this->pipelineStages = pipelineStages;
	util = &Util::instance();
}

CommandBufferSemaphoreInfo::~CommandBufferSemaphoreInfo()
{
}

VkSemaphore CommandBufferSemaphoreInfo::getSemaphore() {
	return this->semaphore;
}

VkSemaphore * CommandBufferSemaphoreInfo::getSemaphorePTR()
{
	return &this->semaphore;
}

bool CommandBufferSemaphoreInfo::getShouldWaitForSignalization() {
	return this->shouldWaitForSignalization;
}

VkPipelineStageFlags* CommandBufferSemaphoreInfo::getPipelineStages() {
	return this->pipelineStages;
}

void CommandBufferSemaphoreInfo::setSemaphore(VkSemaphore semaphore) {
	this->semaphore = semaphore;
}

void CommandBufferSemaphoreInfo::setShouldWaitForSignalization(bool should) {
	this->shouldWaitForSignalization = should;
}

void CommandBufferSemaphoreInfo::setPipelineStages(VkPipelineStageFlags* pipelineStages) {
	this->pipelineStages = pipelineStages;
}
