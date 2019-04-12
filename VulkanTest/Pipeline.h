#pragma once
#ifndef PIPELINE_H
#define PIPELINE_H
#endif
#include <string>
#include <fstream>
#include <vector>
#include <array>
#include "PLATFORM.h"

struct Vertex;
namespace vkglTF {
	struct Vertex;
};
class Vertices;
class IndexBuffer;
template<typename T> class VertexBuffer;
class Renderer;

class Pipeline
{
public:
	Pipeline(VkDevice, VkPhysicalDeviceMemoryProperties, VkRenderPass*, VkSampleCountFlagBits, float, float, VkExtent2D);
	~Pipeline();

	void createPipelineLayout(); 
	void setupViewport(float width, float height, VkExtent2D extent);

	void bindPipeline(VkCommandBuffer, VertexBuffer<Vertex>* = nullptr, IndexBuffer* = nullptr);
	void bindPipelineGLTF(VkCommandBuffer, VertexBuffer<vkglTF::Vertex>* = nullptr, IndexBuffer* = nullptr);

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
	void createMultisamplingInformation(VkSampleCountFlagBits);
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

