#include "pch.h"
#include "DescriptorHandler.h"


DescriptorHandler::DescriptorHandler(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, uint32_t descriptorCount)
{
	this->device = device;
	this->descriptorCount = descriptorCount;
	this->descriptorSetLayout = descriptorSetLayout;

	poolSize.descriptorCount = descriptorCount;
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.maxSets = descriptorCount;
	poolCreateInfo.poolSizeCount = 1;
	poolCreateInfo.pPoolSizes = &poolSize;

	descriptorLayouts.resize(descriptorCount, descriptorSetLayout);
	
	util->ErrorCheck(vkCreateDescriptorPool(device, &poolCreateInfo, nullptr, &pool));
}


DescriptorHandler::~DescriptorHandler()
{
	vkDestroyDescriptorPool(device, pool, nullptr);
}

void DescriptorHandler::createDescriptorSets(std::vector<UniformBuffer*> uniformBuffers)
{
	VkDescriptorSetAllocateInfo createInfo{};

	createInfo.descriptorPool = pool;
	createInfo.descriptorSetCount = descriptorCount;
	createInfo.pSetLayouts = descriptorLayouts.data();
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	createInfo.pNext = nullptr;

	util->ErrorCheck(vkAllocateDescriptorSets(device, &createInfo, descriptorSets.data()));

	for (auto i = 0; i < descriptorCount; ++i) {
		VkDescriptorBufferInfo bufferInfo{};
		VkWriteDescriptorSet writeDescriptor{};

		bufferInfo.buffer = uniformBuffers[i]->getBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = uniformBuffers[i]->getSize();

		descriptorBufferInfos.push_back(bufferInfo);

		writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptor.dstSet = descriptorSets[i];									//Odredisni deskriptor set
		writeDescriptor.dstBinding = 0;												//Iz sejdera
		writeDescriptor.dstArrayElement = 0;										//Prvi index u nizu Deskriptora koji azuriramo. Ne koristimo niz->0
		writeDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;			//Uniform Object Buffer
		writeDescriptor.descriptorCount = 1;										//Koliko elemenata niza azuriramo
		writeDescriptor.pBufferInfo = &bufferInfo;
		writeDescriptor.pImageInfo = nullptr;
		writeDescriptor.pTexelBufferView = nullptr;

		descriptorUpdaters.push_back(writeDescriptor);
	}
}

void DescriptorHandler::updateDescriptorSets()
{
	for (auto i = 0; i < descriptorSets.size(); ++i) {
		vkUpdateDescriptorSets(device, 1, &descriptorUpdaters[i], 0, nullptr);
	}
}

void DescriptorHandler::bind(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout)
{
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSets.data(), 0, nullptr);
}
