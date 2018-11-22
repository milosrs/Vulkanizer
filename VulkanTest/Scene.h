#pragma once
#include "PLATFORM.h"
#include "MainWindow.h"
#include "Util.h"
#include "CommandBuffer.h"
#include "CommandBufferSemaphoreInfo.h"
#include "CommandPool.h"
#include <vector>

class Scene {
public:
	Scene(MainWindow*, Renderer*);
	virtual ~Scene();

	virtual void render(VkViewport*) = 0;
	virtual void recordFrameBuffer(CommandBuffer*, MainWindow*) = 0;
	void createSyncObjects();
	void deleteSyncObjects();
protected:
	std::vector<VkSemaphore> imageAvaiableSemaphores;		//GPU-GPU sync
	std::vector<VkSemaphore> renderFinishedSemaphores;		//GPU-GPU sinhronizacija
	std::vector<VkFence> fences;							//CPU-GPU sinhronizacija

	MainWindow* window = nullptr;
	Renderer* renderer = nullptr;
	std::vector<CommandBuffer*> cmdBuffers;
	CommandPool* cmdPool = nullptr;
	Util* util = nullptr;

	size_t frameCount = 0;
};