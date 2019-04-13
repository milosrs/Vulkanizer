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

	virtual void render();
	void createSyncObjects();
	void deleteSyncObjects();
	void prepareObject(VkCommandPool, VkQueue);
	void setName(std::string);
	void setTextureParams(std::vector<std::string> texturePaths, unsigned int);
							std::string getName();
	
	void recreateDescriptorHandler();
	void rotate(glm::vec2 mouseDelta, glm::vec3 axis);
	virtual void draw(VkCommandBuffer cmdBuffer) = 0;

	bool isObjectReadyToRender();
	IndexBuffer* getIndexBuffer();
	VertexBuffer<Vertex>* getVertexBuffer();
	VertexBuffer<vkglTF::Vertex>* getGLTFVertexBuffer();
	std::vector<Texture*> getTextures();
	DescriptorHandler* getDescriptorHandler();
	Vertices* getVertices();
	std::vector<UniformBuffer*> getUniformBuffers();
	std::vector<VkClearValue>* getClearValues();
	std::vector<std::string> getTexturePaths();
protected:
	void createVideo();
	bool isPrepared = false;
	uint32_t activeImageIndex;
	std::string name;
	unsigned int mode = 0;

	std::vector<VkSemaphore> imageAvaiableSemaphores;		//GPU-GPU sync
	std::vector<VkSemaphore> renderFinishedSemaphores;		//GPU-GPU sinhronizacija
	std::vector<VkFence> fences;							//CPU-GPU sinhronizacija
	std::vector<VkClearValue> clearValues;
	std::vector<UniformBuffer*> uniformBuffers;
	std::vector<Texture*> textures;
	std::vector<std::string> texturePaths;

	std::unique_ptr<Vertices> vertices = nullptr;
	IndexBuffer *indexBuffer = nullptr;
	VertexBuffer<Vertex> *vertexBuffer = nullptr;
	std::unique_ptr<DescriptorHandler> descriptorHandler = nullptr;

	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties memprops = {};
	VkPhysicalDeviceMemoryProperties* pMemprops = {};

	MainWindow* window = nullptr;
	Renderer* renderer = nullptr;
	size_t frameCount = 0;

	//Video data
	const std::string picturePath = "../screnshotsForVideo/";
	const std::string pictureFormat = ".jpg";
	const std::string videoFormat = ".mp4";
	std::vector<std::string> filenames;
	std::vector<std::string> picturenames;

	//glTF data
	VertexBuffer<vkglTF::Vertex> *glTFVertexBuffer = nullptr;
};