#include "pch.h"
#include "Semaphore.h"


Semaphore::Semaphore()
{
	Util& util = Util::instance();

	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCreateSemaphore(util.getDevice(), &semaphoreCreateInfo, nullptr, &semaphore);
}


Semaphore::~Semaphore()
{
	Util& util = Util::instance();
	vkDestroySemaphore(util.getDevice(), this->semaphore, nullptr);
}

VkSemaphore Semaphore::getSemaphore() {
	return this->semaphore;
}
