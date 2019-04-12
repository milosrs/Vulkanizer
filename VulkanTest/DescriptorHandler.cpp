#include "pch.h"
#include "DescriptorHandler.h"
#include "MainWindow.h"
#include "Renderer.h"
#include "UniformBuffer.h"
#include "Util.h"

DescriptorHandler::DescriptorHandler(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, uint32_t descriptorCount)
{
	poolSizes.resize(2);

	this->descriptorSetLayout = descriptorSetLayout;
	this->descriptorLayouts.resize(descriptorCount, descriptorSetLayout);

	poolSizes[0].descriptorCount = descriptorCount;
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	poolSizes[1].descriptorCount = descriptorCount;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.maxSets = descriptorCount;
	poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolCreateInfo.pPoolSizes = poolSizes.data();
	
	Util::ErrorCheck(vkCreateDescriptorPool(device, &poolCreateInfo, nullptr, &pool));
}


DescriptorHandler::~DescriptorHandler()
{
	vkDestroyDescriptorPool(MainWindow::getInstance().getRenderer()->getDevice(), pool, nullptr);
}

void DescriptorHandler::createDescriptorSets(std::vector<UniformBuffer*> uniformBuffers, VkSampler sampler, VkImageView textureView)
{
	VkDescriptorSetAllocateInfo createInfo{};
	descriptorSets.resize(uniformBuffers.size());
	
	createInfo.descriptorPool = pool;
	createInfo.descriptorSetCount = uniformBuffers.size();
	createInfo.pSetLayouts = descriptorLayouts.data();
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	createInfo.pNext = nullptr;

	Util::ErrorCheck(vkAllocateDescriptorSets(MainWindow::getInstance().getRenderer()->getDevice(), &createInfo, descriptorSets.data()));

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

void DescriptorHandler::bind(VkCommandBuffer cmdBuffer, VkDescriptorSet *descriptorSet, VkPipelineLayout pipelineLayout, int index)
{
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSet, 0, nullptr);
}

void DescriptorHandler::updateWritables(std::vector<UniformBuffer*> uniformBuffers, VkSampler sampler, VkImageView textureView)
{
	for (auto i = 0; i < uniformBuffers.size(); ++i) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i]->getBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UBO);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureView;
		imageInfo.sampler = sampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(MainWindow::getInstance().getRenderer()->getDevice(), static_cast<uint32_t>(descriptorWrites.size()),
			descriptorWrites.data(), 0, nullptr);
	}
}

void DescriptorHandler::updateWritables(std::vector<UniformBuffer*> uniformBuffers)
{
	for (uint32_t i = 0; i < uniformBuffers.size(); ++i) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i]->getBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UBO);

		VkWriteDescriptorSet descriptorWrites;

		descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites.dstSet = descriptorSets[i];									//Odredisni deskriptor set
		descriptorWrites.dstBinding = 0;												//Iz sejdera
		descriptorWrites.dstArrayElement = 0;											//Prvi index u nizu Deskriptora koji azuriramo. Ne koristimo niz->0
		descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;			//Uniform Object Buffer
		descriptorWrites.descriptorCount = 1;											//Koliko elemenata niza azuriramo
		descriptorWrites.pBufferInfo = &bufferInfo;
		descriptorWrites.pImageInfo = nullptr;
		descriptorWrites.pTexelBufferView = nullptr;
		descriptorWrites.pNext = nullptr;

		vkUpdateDescriptorSets(MainWindow::getInstance().getRenderer()->getDevice(), 1, &descriptorWrites, 0, nullptr);
	}
}

std::vector<VkDescriptorSet> DescriptorHandler::getDescriptorSets()
{
	return descriptorSets;
}
