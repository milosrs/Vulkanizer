#include "pch.h"
#include "DescriptorHandler.h"

DescriptorHandler::DescriptorHandler(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, uint32_t descriptorCount)
{
	poolSizes.resize(2);

	this->device = device;
	this->descriptorCount = descriptorCount;
	this->descriptorSetLayout = descriptorSetLayout;
	this->descriptorLayouts.resize(descriptorCount, descriptorSetLayout);

	poolSizes[0].descriptorCount = descriptorCount;
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	poolSizes[1].descriptorCount = descriptorCount;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.maxSets = descriptorCount;
	poolCreateInfo.poolSizeCount = poolSizes.size();
	poolCreateInfo.pPoolSizes = poolSizes.data();
	
	Util::ErrorCheck(vkCreateDescriptorPool(device, &poolCreateInfo, nullptr, &pool));
}


DescriptorHandler::~DescriptorHandler()
{
	vkDestroyDescriptorPool(device, pool, nullptr);
}

void DescriptorHandler::createDescriptorSets(std::vector<UniformBuffer*> uniformBuffers, VkSampler sampler, VkImageView textureView)
{
	VkDescriptorSetAllocateInfo createInfo{};
	
	descriptorSets.resize(uniformBuffers.size());

	createInfo.descriptorPool = pool;
	createInfo.descriptorSetCount = descriptorCount;
	createInfo.pSetLayouts = descriptorLayouts.data();
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	createInfo.pNext = nullptr;

	Util::ErrorCheck(vkAllocateDescriptorSets(device, &createInfo, descriptorSets.data()));

	descriptorUpdaters.resize(descriptorCount * 2);
	descriptorBufferInfos.resize(descriptorCount * 2);
	descriptorImageInfos.resize(descriptorCount * 2);

	if (sampler == nullptr && textureView == nullptr) {
		updateWritables(uniformBuffers);
	}
	else if(sampler != nullptr && textureView != nullptr) {
		updateWritables(uniformBuffers, sampler, textureView);
	}
	else {
		throw new std::exception("You cant create a texture without an image sampler or view.");
	}
}

void DescriptorHandler::bind(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout, int index)
{
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[index], 0, nullptr);
}

void DescriptorHandler::updateWritables(std::vector<UniformBuffer*> uniformBuffers, VkSampler sampler, VkImageView textureView)
{
	for (auto i = 0; i < descriptorUpdaters.size(); i+=2) {
		int ind = i / 2;

		descriptorBufferInfos[ind].buffer = uniformBuffers[ind]->getBuffer();
		descriptorBufferInfos[ind].offset = 0;
		descriptorBufferInfos[ind].range = uniformBuffers[ind]->getSize();

		descriptorImageInfos[ind].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptorImageInfos[ind].imageView = textureView;
		descriptorImageInfos[ind].sampler = sampler;

		descriptorUpdaters[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorUpdaters[i].dstSet = descriptorSets[ind];									//Odredisni deskriptor set
		descriptorUpdaters[i].dstBinding = 0;												//Iz sejdera
		descriptorUpdaters[i].dstArrayElement = 0;											//Prvi index u nizu Deskriptora koji azuriramo. Ne koristimo niz->0
		descriptorUpdaters[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;			//Uniform Object Buffer
		descriptorUpdaters[i].descriptorCount = 1;											//Koliko elemenata niza azuriramo
		descriptorUpdaters[i].pBufferInfo = &descriptorBufferInfos[ind];

		descriptorUpdaters[i + 1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorUpdaters[i + 1].dstSet = descriptorSets[ind];
		descriptorUpdaters[i + 1].dstBinding = 1;
		descriptorUpdaters[i + 1].dstArrayElement = 0;
		descriptorUpdaters[i + 1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorUpdaters[i + 1].descriptorCount = 1;
		descriptorUpdaters[i + 1].pImageInfo = &descriptorImageInfos[ind];
	}

	vkUpdateDescriptorSets(device, descriptorUpdaters.size(), descriptorUpdaters.data(), 0, nullptr);
}

void DescriptorHandler::updateWritables(std::vector<UniformBuffer*> uniformBuffers)
{
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
