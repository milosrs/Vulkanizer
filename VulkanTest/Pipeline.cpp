#include "pch.h"
#include "Pipeline.h"


Pipeline::Pipeline(VkDevice* device, VkRenderPass* renderPass)
{
	this->device = device;
	util = &Util::instance();

	auto vertexShaderCode = loadShader("shader.vert");
	auto fragmentShaderCode = loadShader("shader.frag");

	this->createShaderModule(vertexShaderCode);
	this->createShaderModule(fragmentShaderCode);

	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderCreationInfo.data();
	pipelineCreateInfo.pVertexInputState = &this->vertexInputCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &this->inputAssemblyCreateInfo;
	pipelineCreateInfo.pMultisampleState = &this->multisampleCreateInfo;
	pipelineCreateInfo.pDynamicState = &this->dynamicStateCreateInfo;
	pipelineCreateInfo.pColorBlendState = &this->colorBlendCreateInfo;
	pipelineCreateInfo.pRasterizationState = &this->rasterCreateInfo;
	pipelineCreateInfo.layout = this->pipelineLayout;
	pipelineCreateInfo.renderPass = *renderPass;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;

	util->ErrorCheck(vkCreateGraphicsPipelines(*device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline));
}


Pipeline::~Pipeline()
{
	vkDestroyShaderModule(this->device, this->vertexShader, nullptr);
	vkDestroyShaderModule(this->device, this->fragmentShader, nullptr);
	vkDestroyPipelineLayout(this->device, this->pipelineLayout, nullptr);
	vkDestroyPipeline(this->device, this->pipeline, nullptr);
}

void Pipeline::createPipeline() {
	VkPipelineShaderStageCreateInfo vsCreateInfo{};
	VkPipelineShaderStageCreateInfo fsCreateInfo{};

	vsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vsCreateInfo.module = this->vertexShader;										//Pokazivac na sam kod sejdera
	vsCreateInfo.pName = "main";													//Koju funkciju u kodu shadera da pozovem?
	vsCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;								//Koje stanje pajplajna? Za vertex shader to je Vertex Shader Stage

	fsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fsCreateInfo.module = this->fragmentShader;
	fsCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fsCreateInfo.pName = "main";

	this->shaderCreationInfo[0] = vsCreateInfo;
	this->shaderCreationInfo[1] = fsCreateInfo;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	auto result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
	util->ErrorCheck(result);

	if (result != VK_SUCCESS) {
		exit(-1);
	}
}

void Pipeline::setupViewport(float width, float height, VkExtent2D extent) {
	VkViewport viewport;
	VkRect2D scissors;
	VkPipelineViewportStateCreateInfo createInfo{};

	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = width;
	viewport.height = height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissors.offset = { 0,0 };
	scissors.extent = extent;

	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	createInfo.pScissors = &scissors;
	createInfo.pViewports = &viewport;
	createInfo.scissorCount = 1;
	createInfo.viewportCount = 1;

	viewportCreated = true;
}

std::array<VkPipelineShaderStageCreateInfo, 2> Pipeline::getShaderCreationInfo()
{
	return this->shaderCreationInfo;
}

std::vector<char> Pipeline::loadShader(const std::string filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	size_t fileSize = file.tellg();
	std::vector<char> buffer(fileSize);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open a file at location: " + filename);
	}
	else {
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
	}

	return buffer;
}

VkShaderModule Pipeline::createShaderModule(const std::vector<char> code) {
	VkShaderModuleCreateInfo shaderInfo = {};

	shaderInfo.codeSize = code.size();
	shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	util->ErrorCheck(vkCreateShaderModule(this->device, &shaderInfo, nullptr, &shaderHandle));
}

void Pipeline::createVertexInformation() {
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;			//Posto hardkodujemo podatke o tackama, ova polja su nullptr. Pogledaj svesku i sajt.
	vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
	vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
}

void Pipeline::createInputAssemblyInformation() {
	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

void Pipeline::createMultisamplingInformation() {
	multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleCreateInfo.alphaToOneEnable = VK_FALSE;
	multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleCreateInfo.minSampleShading = 1.0f;
	multisampleCreateInfo.pSampleMask = nullptr;
	multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
}

void Pipeline::setupRasterizer() {
	rasterCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterCreateInfo.depthBiasClamp = 0.0f;
	rasterCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterCreateInfo.depthBiasEnable = VK_FALSE;
	rasterCreateInfo.depthBiasSlopeFactor = 0.0f;
	rasterCreateInfo.depthClampEnable = VK_FALSE;
	rasterCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterCreateInfo.lineWidth = 1.0f;
	rasterCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterCreateInfo.rasterizerDiscardEnable = VK_FALSE;
}

void Pipeline::setupColorBlending() {
	VkPipelineColorBlendAttachmentState attachment{};
	

	attachment.colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_R_BIT;
	attachment.blendEnable = VK_FALSE;
	attachment.colorBlendOp = VK_BLEND_OP_ADD;
	attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlendCreateInfo.attachmentCount = 1;
	colorBlendCreateInfo.pAttachments = &attachment;
	colorBlendCreateInfo.blendConstants[0] = 0.0f; // Optional
	colorBlendCreateInfo.blendConstants[1] = 0.0f; // Optional
	colorBlendCreateInfo.blendConstants[2] = 0.0f; // Optional
	colorBlendCreateInfo.blendConstants[3] = 0.0f; // Optional
}

void Pipeline::setupDynamicState() {
	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.dynamicStateCount = 2;
	dynamicStateCreateInfo.pDynamicStates = dynamicStates;
}