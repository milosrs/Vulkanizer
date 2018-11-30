#pragma once
#include "Buffer.h"
#include "PLATFORM.h"

class VertexBuffer : public Buffer
{
public:
	VertexBuffer(VkDevice, VkPhysicalDeviceMemoryProperties, std::shared_ptr<Vertices>);
	~VertexBuffer();
	void fillBuffer();

	std::vector<Vertex> getVertices();
	std::vector<uint16_t> getIndices();
private:
	std::shared_ptr<Vertices> vertices = nullptr;
};

