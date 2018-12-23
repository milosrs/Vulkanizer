#pragma once
#ifdef DESCRIPTOR_HANDLER_H
#define DESCRIPTOR_HANDLER_H
#endif
#include "PLATFORM.h"
#include "UniformBuffer.h"
#include "Util.h"
#include <vector>

class DescriptorHandler
{
public:
	/*
		VkDevice: Device to bind the pool to
		VkDescriptorSetLayout: Descriptor layout from pipeline
		uint32_t: Descriptor Count
	 */
	DescriptorHandler(VkDevice, VkDescriptorSetLayout, uint32_t);
	~DescriptorHandler();

	void createDescriptorSets(std::vector<UniformBuffer*>, VkSampler = nullptr, VkImageView = nullptr);
	void bind(VkCommandBuffer, VkPipelineLayout, int);
private:
	void updateWritables(std::vector<UniformBuffer*>, VkSampler, VkImageView);
	void updateWritables(std::vector<UniformBuffer*>);

	VkDescriptorPool pool;
	std::vector<VkDescriptorPoolSize> poolSizes = {};
	VkDescriptorPoolCreateInfo poolCreateInfo = {};

	std::vector<VkDescriptorSetLayout> descriptorLayouts;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
	std::vector<VkDescriptorImageInfo> descriptorImageInfos;
	std::vector<VkWriteDescriptorSet> descriptorUpdaters;

	uint32_t descriptorCount = -1;
	VkDescriptorSetLayout descriptorSetLayout{};
	VkDevice device = VK_NULL_HANDLE;
};

