#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <array>
#include "PLATFORM.h"
#include "Util.h"

class Pipeline
{
public:
	Pipeline(VkDevice* device, VkRenderPass* renderPass);
	~Pipeline();

	void createPipeline(); 
	void setupViewport(float width, float height, VkExtent2D extent);

	void bindPipeline(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);

	std::array<VkPipelineShaderStageCreateInfo, 2> getShaderCreationInfo();
private:
	std::vector<char> loadShader(const std::string filename);
	void createShaderModule(const std::vector<char> code);			//Da, ovde ide kod iz shadera.

	void setupDynamicState();
	void setupColorBlending();
	void setupRasterizer();
	void createMultisamplingInformation();
	void createInputAssemblyInformation();
	void createVertexInformation();

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderCreationInfo;

	VkShaderModule vertexShader = VK_NULL_HANDLE;
	VkShaderModule fragmentShader = VK_NULL_HANDLE;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{}; 
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
	VkPipelineRasterizationStateCreateInfo rasterCreateInfo{};
	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};

	VkDevice device = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	VkShaderModule shaderHandle = {};
	bool viewportCreated = false;

	Util* util = nullptr;
};

