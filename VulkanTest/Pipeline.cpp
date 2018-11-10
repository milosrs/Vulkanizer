#include "pch.h"
#include "Pipeline.h"


Pipeline::Pipeline(VkDevice device)
{
	this->device = device;
	util = &Util::instance();

	auto vertexShaderCode = loadShader("shader.vert");
	auto fragmentShaderCode = loadShader("shader.frag");

	this->createShaderModule(vertexShaderCode, &this->vertexShader);
	this->createShaderModule(fragmentShaderCode, &this->fragmentShader);
}


Pipeline::~Pipeline()
{
	vkDestroyShaderModule(device, this->vertexShader, nullptr);
	vkDestroyShaderModule(device, this->fragmentShader, nullptr);
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

	auto result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipeline);
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

VkShaderModule Pipeline::createShaderModule(const std::vector<char> code, VkShaderModule* shaderHandle) {
	VkShaderModuleCreateInfo shaderInfo = {};

	shaderInfo.codeSize = code.size();
	shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	util->ErrorCheck(vkCreateShaderModule(this->device, &shaderInfo, nullptr, shaderHandle));
}

void createVertexInformation() {
	VkPipelineVertexInputStateCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	createInfo.pVertexAttributeDescriptions = nullptr;			//Posto hardkodujemo podatke o tackama, ova polja su nullptr. Pogledaj svesku i sajt.
	createInfo.vertexAttributeDescriptionCount = 0;
	createInfo.pVertexBindingDescriptions = nullptr;
	createInfo.vertexBindingDescriptionCount = 0;
}

void createInputAssemblyInformation() {
	VkPipelineInputAssemblyStateCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	createInfo.primitiveRestartEnable = VK_FALSE;
	createInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

void createMultisamplingInformation() {
	VkPipelineMultisampleStateCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	createInfo.alphaToOneEnable = VK_FALSE;
	createInfo.alphaToCoverageEnable = VK_FALSE;
	createInfo.sampleShadingEnable = VK_FALSE;
	createInfo.minSampleShading = 1.0f;
	createInfo.pSampleMask = nullptr;
	createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
}

void setupRasterizer() {
	VkPipelineRasterizationStateCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	createInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	createInfo.depthBiasClamp = 0.0f;
	createInfo.depthBiasConstantFactor = 0.0f;
	createInfo.depthBiasEnable = VK_FALSE;
	createInfo.depthBiasSlopeFactor = 0.0f;
	createInfo.depthClampEnable = VK_FALSE;
	createInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	createInfo.lineWidth = 1.0f;
	createInfo.polygonMode = VK_POLYGON_MODE_FILL;
	createInfo.rasterizerDiscardEnable = VK_FALSE;
}

void setupColorBlending() {
	VkPipelineColorBlendAttachmentState attachment{};
	VkPipelineColorBlendStateCreateInfo createInfo{};

	attachment.colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_R_BIT;
	attachment.blendEnable = VK_FALSE;
	attachment.colorBlendOp = VK_BLEND_OP_ADD;
	attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	createInfo.logicOpEnable = VK_FALSE;
	createInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &attachment;
	createInfo.blendConstants[0] = 0.0f; // Optional
	createInfo.blendConstants[1] = 0.0f; // Optional
	createInfo.blendConstants[2] = 0.0f; // Optional
	createInfo.blendConstants[3] = 0.0f; // Optional
}

void setupDynamicState() {
	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;
}