#include "pch.h"
#include <iostream>
#include "vulkan\vulkan.h"
#include "Renderer.h"
#include "CommandPool.h"
#include "Util.h"
#include "CommandBuffer.h"
#include "Semaphore.h"

/*vkCmdPipelineBarrier - kreiramo barijeru nad nekim delovima pajplajna. Ako dodjemo do trazenog dela pajplajna, pajplajn se zaustavlja dok se posao ne zavrsi.*/
/*asdf*/
int main()
{
	Util& util = Util::instance();
	Renderer renderer = Renderer();
	CommandPool cmdPool = CommandPool();
	Semaphore semaphore = Semaphore();
	
	util.setDevice(renderer.getDevice());
	util.setGraphicsFamilyIndex(renderer.getGraphicsFamilyIndex());

	
	util.setCommandPool(cmdPool.getCommandPool());

	CommandBuffer cmdBuffer = CommandBuffer(util.getCommandPool());
	cmdBuffer.submitQueue(renderer.getQueue());

	VkFence fence = cmdBuffer.getFence();
	vkWaitForFences(util.getDevice(), 1, &fence, VK_TRUE, UINT64_MAX);

	return 0;
}