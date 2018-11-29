#pragma once
#include "PLATFORM.h"
#include "Vertices.h"
#include "Util.h"

class VertexBuffer
{
public:
	VertexBuffer(VkDevice, VkPhysicalDeviceMemoryProperties);
	~VertexBuffer();

	void initVertexBuffer(std::shared_ptr<Vertices> vertices);
	void fillBuffer();
	VkBuffer getBuffer();
	std::vector<Vertex> getVertices();
private:
	VkBuffer buffer = VK_NULL_HANDLE;;
	VkDevice device = VK_NULL_HANDLE;;
	VkMemoryRequirements memoryRequirements = {};
	VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;;
	VkPhysicalDeviceMemoryProperties deviceMemoryProps = {};

	VkMemoryAllocateInfo allocateInfo{};
	VkBufferCreateInfo bufferInfo{};

	Util* util = nullptr;
	std::shared_ptr<Vertices> vertices = nullptr;
};

