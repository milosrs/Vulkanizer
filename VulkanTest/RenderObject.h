#pragma once
#include "PLATFORM.h"
#include "MainWindow.h"
#include "CommandBufferSemaphoreInfo.h"
#include "CommandBufferHandler.h"
#include <vector>

class RenderObject {
public:
	RenderObject(MainWindow*, Renderer*, std::string = "");
	virtual ~RenderObject();

	virtual void render(VkViewport*);
	void createSyncObjects();
	void deleteSyncObjects();
	void prepareObject(std::string, unsigned int, VkCommandPool, VkQueue);
	void setName(std::string);
	std::string getName();
	bool isObjectReadyToRender();

	IndexBuffer* getIndexBuffer();
	VertexBuffer* getVertexBuffer();
	Texture* getTexture();
	DescriptorHandler* getDescriptorHandler();

	std::shared_ptr<Vertices> getVertices();
	std::vector<UniformBuffer*> getUniformBuffers();
	std::vector<VkClearValue>* getClearValues();
protected:
	bool isPrepared = false;
	std::string name;

	std::shared_ptr<Vertices> vertices = nullptr;
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