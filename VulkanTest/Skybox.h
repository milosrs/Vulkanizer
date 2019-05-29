#include <vector>
#include <filesystem>
#include <algorithm>
#include <memory>
#include "PLATFORM.h"
#include "RenderObject.h"
#include "Vertices.h"

class Texture;
template class VertexBuffer<Vertex>;
template class StagingBuffer<unsigned char>;

#pragma once
class Skybox : RenderObject
{
public:
	Skybox(std::string);
	~Skybox();

	Texture getFace(glm::vec3);
private:
	VkDeviceSize cubemapSize;
	VkDeviceSize oneFaceSize;
	VkDevice device = VK_NULL_HANDLE;
	VkSamplerCreateInfo samplerInfo{};
	VkPhysicalDeviceMemoryProperties *memprops = nullptr;
	Texture* faces[6];				//UP, DOWN, LEFT, RIGHT, BACK, FORWARD
	
	StagingBuffer<unsigned char> *stagingBuffer = nullptr;
	VertexBuffer<Vertex> *vertexBuffer = nullptr;

	int getFaceArrayPositionByFilename(std::string filename);
	void createSampler();
};

