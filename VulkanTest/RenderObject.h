#pragma once
#include "PLATFORM.h"
#include "MainWindow.h"
#include "CommandBufferSemaphoreInfo.h"
#include "CommandBufferHandler.h"
#include <vector>

class RenderObject {
public:
	/*
		1. MainWindow, 
		2. Renderer,
		3. Name, 
		4. TexturePath, 
		5. TextureMode
	*/
	RenderObject(MainWindow*, Renderer*, std::string = "");
	virtual ~RenderObject();

	virtual void render(VkViewport*);
	void createSyncObjects();
	void deleteSyncObjects();
	void prepareObject(VkCommandPool, VkQueue);
	void setName(std::string);
	/*
		1. Texture path,
		2. Mode
	*/
	void setTextureParams(std::string, unsigned int);
	std::string getName();
	bool isObjectReadyToRender();

	IndexBuffer* getIndexBuffer();
	VertexBuffer* getVertexBuffer();
	Texture* getTexture();
	DescriptorHandler* getDescriptorHandler();
	Vertices* getVertices();

	std::vector<UniformBuffer*> getUniformBuffers();
	std::vector<VkClearValue>* getClearValues();
protected:
	bool isPrepared = false;

	std::string name;
	std::string texturePath = "";
	unsigned int mode = 0;

	std::unique_ptr<Vertices> vertices = nullptr;
	std::vector<VkSemaphore> imageAvaiableSemaphores;		//GPU-GPU sync
	std::vector<VkSemaphore> renderFinishedSemaphores;		//GPU-GPU sinhronizacija
	std::vector<VkFence> fences;							//CPU-GPU sinhronizacija
	std::vector<VkClearValue> clearValues;
	std::vector<UniformBuffer*> uniformBuffers;

	std::unique_ptr<IndexBuffer> indexBuffer = nullptr;
	std::unique_ptr<VertexBuffer> vertexBuffer = nullptr;
	std::unique_ptr<Texture> texture = nullptr;
	std::unique_ptr<DescriptorHandler> descriptorHandler = nullptr;

	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties memprops = {};
	VkPhysicalDeviceMemoryProperties* pMemprops = {};

	MainWindow* window = nullptr;
	Renderer* renderer = nullptr;

	size_t frameCount = 0;
};