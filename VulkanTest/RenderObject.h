#pragma once
#include "PLATFORM.h"
#include <vector>
#include <stdio.h>
#include <filesystem>
#include <iostream>
#include "Vertices.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "DescriptorHandler.h"

class MainWindow;
class Renderer;
class Texture;
class UniformBuffer;
class WindowController;
class CommandBufferSemaphoreInfo;

struct Vertex;
namespace vkglTF {
	struct Vertex;
};

class RenderObject {
public:
	RenderObject(const RenderObject&) = delete;
	void operator=(RenderObject const&) = delete;
	RenderObject(std::string = "");
	virtual ~RenderObject();

	void prepareObject(VkCommandPool, VkQueue);
	void setName(std::string);
	void setTextureParams(std::vector<std::string> texturePaths, unsigned int);
							std::string getName();
	
	void rotate(glm::vec2 mouseDelta, glm::vec3 axis);
	virtual void draw(VkCommandBuffer cmdBuffer) = 0;

	bool isObjectReadyToRender();
	IndexBuffer* getIndexBuffer();
	VertexBuffer<Vertex>* getVertexBuffer();
	VertexBuffer<vkglTF::Vertex>* getGLTFVertexBuffer();
	std::vector<Texture*> getTextures();
	Vertices* getVertices();
	std::vector<UniformBuffer*> getUniformBuffers();
	std::vector<std::string> getTexturePaths();
protected:
	bool isPrepared = false;
	std::string name;
	unsigned int mode = 0;

	std::vector<UniformBuffer*> uniformBuffers;
	std::vector<Texture*> textures;
	std::vector<std::string> texturePaths;

	std::unique_ptr<Vertices> vertices = nullptr;
	IndexBuffer *indexBuffer = nullptr;
	VertexBuffer<Vertex> *vertexBuffer = nullptr;

	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties memprops = {};
	VkPhysicalDeviceMemoryProperties* pMemprops = {};

	MainWindow* window = nullptr;
	Renderer* renderer = nullptr;
	
	//glTF data
	VertexBuffer<vkglTF::Vertex> *glTFVertexBuffer = nullptr;
};