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