#pragma once
#include "PLATFORM.h"
#include "MainWindow.h"
#include "CommandBufferSemaphoreInfo.h"
#include "CommandBufferHandler.h"
#include <vector>

class Scene {
public:
	Scene(MainWindow*, Renderer*);
	virtual ~Scene();

	virtual void render(VkViewport*) = 0;
	virtual void recordFrameBuffer() = 0;
	void createSyncObjects();
	void deleteSyncObjects();
protected:
	std::shared_ptr<Vertices> vertices = nullptr;
	std::vector<VkSemaphore> imageAvaiableSemaphores;		//GPU-GPU sync
	std::vector<VkSemaphore> renderFinishedSemaphores;		//GPU-GPU sinhronizacija
	std::vector<VkFence> fences;							//CPU-GPU sinhronizacija

	MainWindow* window = nullptr;
	Renderer* renderer = nullptr;

	size_t frameCount = 0;
};