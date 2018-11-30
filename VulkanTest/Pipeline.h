#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <array>
#include "PLATFORM.h"
#include "Util.h"
#include "Vertices.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Renderer.h"

class Pipeline
{
public:
	Pipeline(VkDevice, VkPhysicalDeviceMemoryProperties, VkRenderPass*, float, float, VkExtent2D);
	~Pipeline();

	void createPipelineLayout(); 
	void setupViewport(float width, float height, VkExtent2D extent);

	void bindPipeline(VkCommandBuffer, VertexBuffer* = nullptr, IndexBuffer* = nullptr);

	VkViewport getViewport();
	VkViewport* getViewportPTR();

	std::array<VkPipelineShaderStageCreateInfo, 2> getShaderCreationInfo();
private:
	std::vector<char> loadShader(const std::string filename);
	void createShaderModule(const std::vector<char> code, VkShaderModule* shaderHandle);			//Oovde ide kod iz shadera, ali ne .frag i .vert nego SPIR assembly kod.

	void createDynamicState();
	void createColorBlending();
	void createRasterizer();
	void createMultisamplingInformation();
	void createInputAssemblyInformation();
	void createVertexInformation();

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderCreationInfo;

	VkShaderModule vertexShader = VK_NULL_HANDLE;
	VkShaderModule fragmentShader = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties memprops;

	VkVertexInputBindingDescription bindingDescription;
	std::vector<VkVertexInputAttributeDescription> attributeDescription;

	VkViewport viewport = {};
	VkRect2D scissors = {};

	VkPipelineColorBlendAttachmentState attachment{};
	VkDynamicState dynamicStates[2] {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineShaderStageCreateInfo vsCreateInfo{};
	VkPipelineShaderStageCreateInfo fsCreateInfo{};
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{}; 
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
	VkPipelineRasterizationStateCreateInfo rasterCreateInfo{};
	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};

	VkDevice device = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline pipeline = VK_NULL_HANDLE;
	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	bool viewportCreated = false;

	Util* util = nullptr;
};

