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
	Pipeline(VkDevice* device, VkRenderPass* renderPass, float width, float height, VkExtent2D extent);
	~Pipeline();

	void createPipelineLayout(); 
	void setupViewport(float width, float height, VkExtent2D extent);

	void bindPipeline(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);

	VkViewport getViewport();
	VkViewport* getViewportPTR();

	std::array<VkPipelineShaderStageCreateInfo, 2> getShaderCreationInfo();
private:
	std::vector<char> loadShader(const std::string filename);
	void createShaderModule(const std::vector<char> code);			//Da, ovde ide kod iz shadera.

	void createDynamicState();
	void createColorBlending();
	void createRasterizer();
	void createMultisamplingInformation();
	void createInputAssemblyInformation();
	void createVertexInformation();

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderCreationInfo;

	VkShaderModule vertexShader = VK_NULL_HANDLE;
	VkShaderModule fragmentShader = VK_NULL_HANDLE;

	VkViewport viewport = {};
	VkRect2D scissors = {};

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{}; 
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
	VkPipelineRasterizationStateCreateInfo rasterCreateInfo{};
	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};

	VkDevice device = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	VkShaderModule shaderHandle = {};
	bool viewportCreated = false;

	Util* util = nullptr;
};

