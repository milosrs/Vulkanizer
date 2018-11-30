#include "pch.h"
#include "Pipeline.h"


Pipeline::Pipeline(VkDevice device, VkPhysicalDeviceMemoryProperties memprops, VkRenderPass* renderPass, 
					float width, float height, VkExtent2D extent)
{
	this->device = device;
	this->memprops = memprops;
	util = &Util::instance();

	auto vertexShaderCode = loadShader("vert.spv");
	auto fragmentShaderCode = loadShader("frag.spv");

	this->createShaderModule(vertexShaderCode, &vertexShader);
	this->createShaderModule(fragmentShaderCode, &fragmentShader);
	this->createInputAssemblyInformation();
	this->setupViewport(width, height, extent);
	this->createMultisamplingInformation();
	this->createVertexInformation();
	this->createColorBlending();
	this->createDynamicState();
	this->createRasterizer();

	this->createPipelineLayout();

	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pVertexInputState = &this->vertexInputCreateInfo;
	pipelineCreateInfo.pStages = shaderCreationInfo.data();
	pipelineCreateInfo.pMultisampleState = &this->multisampleCreateInfo;
	pipelineCreateInfo.pDynamicState = nullptr;
	pipelineCreateInfo.pColorBlendState = &this->colorBlendCreateInfo;
	pipelineCreateInfo.pRasterizationState = &this->rasterCreateInfo;
	pipelineCreateInfo.layout = this->pipelineLayout;
	pipelineCreateInfo.renderPass = *renderPass;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;
	pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &this->inputAssemblyCreateInfo;

	util->ErrorCheck(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline));
}

Pipeline::~Pipeline()
{
	vkDestroyShaderModule(this->device, this->vertexShader, nullptr);
	vkDestroyShaderModule(this->device, this->fragmentShader, nullptr);
	vkDestroyPipelineLayout(this->device, this->pipelineLayout, nullptr);
	vkDestroyPipeline(this->device, this->pipeline, nullptr);
}

void Pipeline::createPipelineLayout() {
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
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = width;
	viewport.height = height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissors.offset = { 0,0 };
	scissors.extent = extent;

	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.pScissors = &scissors;
	viewportStateCreateInfo.pViewports = &viewport;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.viewportCount = 1;

	viewportCreated = true;
}

void Pipeline::bindPipeline(VkCommandBuffer commandBuffer, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	if (vertexBuffer != nullptr) {
		VkBuffer vertexBuffers[] = { vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	}

	if (indexBuffer != nullptr) {
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
	}
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

void Pipeline::createShaderModule(const std::vector<char> code, VkShaderModule* shaderHandle) {
	VkShaderModuleCreateInfo shaderInfo = {};

	shaderInfo.codeSize = code.size();
	shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	util->ErrorCheck(vkCreateShaderModule(this->device, &shaderInfo, nullptr, shaderHandle));
}

void Pipeline::createVertexInformation() {
	bindingDescription = Vertex::getBindingDescription();
	auto attributes = Vertex::getAttributeDescriptions();
	attributeDescription.insert(attributeDescription.begin(), attributes.begin(), attributes.end());

	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescription.data();			//Vise ne hardkodujemo podatke o tackama. Dobijamo ih iz sejdera. --->Deprecated: Posto hardkodujemo podatke o tackama, ova polja su nullptr. Pogledaj svesku i sajt.
	vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
	vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
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

void Pipeline::createRasterizer() {
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

void Pipeline::createColorBlending() {
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

void Pipeline::createDynamicState() {
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.dynamicStateCount = 2;
	dynamicStateCreateInfo.pDynamicStates = dynamicStates;
}

VkViewport Pipeline::getViewport()
{
	return this->viewport;
}

VkViewport * Pipeline::getViewportPTR()
{
	return &this->viewport;
}

std::array<VkPipelineShaderStageCreateInfo, 2> Pipeline::getShaderCreationInfo()
{
	return this->shaderCreationInfo;
}