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
	Pipeline(VkDevice device);
	~Pipeline();

	void createPipeline(); 
	void setupViewport(float width, float height, VkExtent2D extent);


	std::array<VkPipelineShaderStageCreateInfo, 2> getShaderCreationInfo();
private:
	std::vector<char> loadShader(const std::string filename);
	VkShaderModule createShaderModule(const std::vector<char> code, VkShaderModule* shaderHandle);			//Da, ovde ide kod iz shadera.

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderCreationInfo;

	VkShaderModule vertexShader = VK_NULL_HANDLE;
	VkShaderModule fragmentShader = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkPipelineLayout pipeline;
	bool viewportCreated = false;

	Util* util = nullptr;
};

