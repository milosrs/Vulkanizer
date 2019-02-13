#pragma once
#include "PLATFORM.h"
#include "MainWindow.h"
#include "CommandBufferSemaphoreInfo.h"
#include "CommandBufferHandler.h"
#include <vector>
#include <stdio.h>
#include <filesystem>
#include <iostream>

class WindowController;

class RenderObject {
public:
	RenderObject(std::string = "");
	virtual ~RenderObject();

	virtual void render();
	void createSyncObjects();
	void deleteSyncObjects();
	void prepareObject(VkCommandPool, VkQueue);
	void setName(std::string);
	void setTextureParams(std::vector<std::string> texturePaths, unsigned int);
							std::string getName();
	bool isObjectReadyToRender();
	void recreateDescriptorHandler();

	void rotate(glm::vec2 mouseDelta);

	IndexBuffer* getIndexBuffer();
	VertexBuffer* getVertexBuffer();
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

	std::unique_ptr<Vertices> vertices = nullptr;
	std::vector<VkSemaphore> imageAvaiableSemaphores;		//GPU-GPU sync
	std::vector<VkSemaphore> renderFinishedSemaphores;		//GPU-GPU sinhronizacija
	std::vector<VkFence> fences;							//CPU-GPU sinhronizacija
	std::vector<VkClearValue> clearValues;
	std::vector<UniformBuffer*> uniformBuffers;
	std::vector<Texture*> textures;
	std::vector<std::string> texturePaths;

	std::unique_ptr<IndexBuffer> indexBuffer = nullptr;
	std::unique_ptr<VertexBuffer> vertexBuffer = nullptr;
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
};