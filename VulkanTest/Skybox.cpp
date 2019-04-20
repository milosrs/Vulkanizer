#include "pch.h"
#include "Skybox.h"
#include "Texture.h"
#include "MainWindow.h"
#include "Renderer.h"
#include "Util.h"
#include "StagingBuffer.h"

Skybox::Skybox(std::string facesDirectory)
{
	createSampler();
	device = MainWindow::getInstance().getRenderer()->getDevice();
	*memprops = MainWindow::getInstance().getRenderer()->getPhysicalDeviceMemoryProperties();

	if (std::filesystem::is_directory(facesDirectory)) {
		for (auto &file : std::filesystem::directory_iterator(facesDirectory)) {
			int arrayPosition = -1;
			std::string filename = file.path().filename().generic_string();
			
			std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
			arrayPosition = getFaceArrayPositionByFilename(filename);

			if (arrayPosition != -1) {
				faces[arrayPosition] = new Texture(device, memprops, VK_FORMAT_R8G8B8A8_UNORM, 
													file.path().root_path().generic_string(), 4, &samplerInfo);
			}
		}
	}

	std::vector<unsigned char> allTexturesData;
	cubemapSize = faces[0]->getFullDimension() * 6;
	oneFaceSize = faces[0]->getFullDimension();
	stagingBuffer = new StagingBuffer<unsigned char>(device, *memprops, cubemapSize);

	for (int i = 0; i < 6; i++) {
		std::vector<unsigned char> pixels(faces[i]->getPixels()[0], faces[i]->getPixels()[faces[i]->getFullDimension() - 1]);
		allTexturesData.insert(allTexturesData.end(), pixels.begin(), pixels.end());
	}

	stagingBuffer->fillBuffer(allTexturesData);
}

Skybox::~Skybox()
{
}

Texture Skybox::getFace(glm::vec3 directionVector)
{
	directionVector = glm::normalize(directionVector);

	if (directionVector.x > 0) {
		return *faces[SkyboxFaces::TOP];
	}
	else if (directionVector.x < 0) {
		return *faces[SkyboxFaces::BOTTOM];
	}
	else if (directionVector.y > 0) {
		return *faces[SkyboxFaces::LEFT];
	}
	else if (directionVector.y < 0) {
		return *faces[SkyboxFaces::RIGHT];
	}
	else if (directionVector.z > 0) {
		return *faces[SkyboxFaces::FORWARD];
	}
	else if (directionVector.z < 0) {
		return *faces[SkyboxFaces::BACK];
	}
}

int Skybox::getFaceArrayPositionByFilename(std::string filename)
{
	int ret = -1;

	if (filename.find("top") != std::string::npos) {
		ret = 0;
	}
	else if (filename.find("bottom") != std::string::npos) {
		ret = 1;
	}
	else if (filename.find("left") != std::string::npos) {
		ret = 2;
	}
	else if (filename.find("right") != std::string::npos) {
		ret = 3;
	}
	else if (filename.find("forward") != std::string::npos) {
		ret = 4;
	}
	else if (filename.find("back") != std::string::npos) {
		ret = 5;
	}

	return ret;
}

void Skybox::createSampler()
{
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
}
