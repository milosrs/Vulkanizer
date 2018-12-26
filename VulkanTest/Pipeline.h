/*Pipeline ne mora da cuva reference na create info-ove, tako da samo ih vracaj u private fjama.*/
#pragma once
#ifndef PIPELINE_H
#define PIPELINE_H
#endif
#include <string>
#include <fstream>
#include <vector>
#include <array>
#include "PLATFORM.h"
#include "Vertices.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "Util.h"

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

	VkDescriptorSetLayout getDescriptorSetLayout();
	VkPipelineLayout getPipelineLayout();

	std::array<VkPipelineShaderStageCreateInfo, 2> getShaderCreationInfo();
private:
	std::vector<char> loadShader(const std::string filename);
	void createShaderModule(const std::vector<char> code, VkShaderModule* shaderHandle);			//Oovde ide kod iz shadera, ali ne .frag i .vert nego SPIR assembly kod.

	void createDescriptorLayout();
	void createDynamicState();
	void createColorBlending();
	void createRasterizer();
	void createMultisamplingInformation();
	void createInputAssemblyInformation();
	void createVertexInformation();
	void createDepthStencilInformation();

	/*Shaders*/
	std::array<VkPipelineShaderStageCreateInfo, 2> shaderCreationInfo;
	VkShaderModule vertexShader = VK_NULL_HANDLE;
	VkShaderModule fragmentShader = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties memprops;

	/*Vertex Buffer Binding*/
	VkVertexInputBindingDescription bindingDescription;
	std::vector<VkVertexInputAttributeDescription> attributeDescription;

	/*Uniform Buffer Object binding*/
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {};
	VkDescriptorSetLayout descriptorLayout = {};

	VkViewport viewport = {};
	VkRect2D scissors = {};

	VkPipelineColorBlendAttachmentState attachment{};
	VkDynamicState dynamicStates[2] {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDepthStencilStateCreateInfo dsCreateInfo{};
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
};

