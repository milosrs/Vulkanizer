#pragma once
#ifdef DESCRIPTOR_HANDLER_H
#define DESCRIPTOR_HANDLER_H
#endif
#include <vector>
#include "PLATFORM.h"


class MainWindow;
class Renderer;
class UniformBuffer;

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
	static void bind(VkCommandBuffer, VkDescriptorSet*, VkPipelineLayout, int);
	void updateWritables(std::vector<UniformBuffer*>, VkSampler, VkImageView);
	void updateWritables(std::vector<UniformBuffer*>);
	std::vector<VkDescriptorSet> getDescriptorSets();
private:
	VkDescriptorPool pool;
	std::vector<VkDescriptorPoolSize> poolSizes = {};
	VkDescriptorPoolCreateInfo poolCreateInfo = {};

	std::vector<VkDescriptorSetLayout> descriptorLayouts;
	std::vector<VkDescriptorSet> descriptorSets;

	VkDescriptorSetLayout descriptorSetLayout{};
};

