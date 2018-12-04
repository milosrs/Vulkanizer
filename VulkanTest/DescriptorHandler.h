#pragma once
#include "PLATFORM.h"
#include "Util.h"
#include "UniformBuffer.h"
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

	void updateDescriptorSets();
	void createDescriptorSets(std::vector<UniformBuffer*>);
	void bind(VkCommandBuffer, VkPipelineLayout);
private:

	VkDescriptorPool pool = VK_NULL_HANDLE;
	VkDescriptorPoolSize poolSize = {};
	VkDescriptorPoolCreateInfo poolCreateInfo = {};

	std::vector<VkDescriptorSetLayout> descriptorLayouts;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
	std::vector<VkWriteDescriptorSet> descriptorUpdaters;

	uint32_t descriptorCount = -1;
	VkDescriptorSetLayout descriptorSetLayout{};
	VkDevice device = VK_NULL_HANDLE;


	Util* util = &Util::instance();
};

