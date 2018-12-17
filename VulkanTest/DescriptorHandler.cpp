#include "pch.h"
#include "DescriptorHandler.h"


DescriptorHandler::DescriptorHandler(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, uint32_t descriptorCount)
{
	this->device = device;
	this->descriptorCount = descriptorCount;
	this->descriptorSetLayout = descriptorSetLayout;
	this->descriptorLayouts.resize(descriptorCount, descriptorSetLayout);

	poolSize.descriptorCount = descriptorCount;
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.maxSets = descriptorCount;
	poolCreateInfo.poolSizeCount = 1;
	poolCreateInfo.pPoolSizes = &poolSize;
	
	util->ErrorCheck(vkCreateDescriptorPool(device, &poolCreateInfo, nullptr, &pool));
}


DescriptorHandler::~DescriptorHandler()
{
	vkDestroyDescriptorPool(device, pool, nullptr);
}

void DescriptorHandler::createDescriptorSets(std::vector<UniformBuffer*> uniformBuffers)
{
	VkDescriptorSetAllocateInfo createInfo{};
	
	descriptorSets.resize(uniformBuffers.size());

	createInfo.descriptorPool = pool;
	createInfo.descriptorSetCount = descriptorCount;
	createInfo.pSetLayouts = descriptorLayouts.data();
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	createInfo.pNext = nullptr;

	util->ErrorCheck(vkAllocateDescriptorSets(device, &createInfo, descriptorSets.data()));

	descriptorUpdaters.resize(descriptorCount);
	descriptorBufferInfos.resize(descriptorCount);

 	for (auto i = 0; i < descriptorCount; ++i) {
		descriptorBufferInfos[i].buffer = uniformBuffers[i]->getBuffer();
		descriptorBufferInfos[i].offset = 0;
		descriptorBufferInfos[i].range = uniformBuffers[i]->getSize();

		descriptorUpdaters[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorUpdaters[i].dstSet = descriptorSets[i];									//Odredisni deskriptor set
		descriptorUpdaters[i].dstBinding = 0;												//Iz sejdera
		descriptorUpdaters[i].dstArrayElement = 0;											//Prvi index u nizu Deskriptora koji azuriramo. Ne koristimo niz->0
		descriptorUpdaters[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;			//Uniform Object Buffer
		descriptorUpdaters[i].descriptorCount = 1;											//Koliko elemenata niza azuriramo
		descriptorUpdaters[i].pBufferInfo = &descriptorBufferInfos[i];
		descriptorUpdaters[i].pImageInfo = nullptr;
		descriptorUpdaters[i].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(device, 1, &descriptorUpdaters[i], 0, nullptr);
	}
}

void DescriptorHandler::bind(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout, int index)
{
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[index], 0, nullptr);
}
