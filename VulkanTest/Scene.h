#pragma once
#include "PLATFORM.h"
#include "MainWindow.h"
#include "Util.h"
#include "CommandBuffer.h"
#include "CommandPool.h"

class Scene {
public:
	Scene(MainWindow*, Renderer*, CommandBuffer*, CommandPool*);

	virtual void render() = 0;
	virtual void recordFrameBuffer(CommandBuffer*, MainWindow*) = 0;
protected:
	MainWindow* window = nullptr;
	Renderer* renderer = nullptr;
	CommandBuffer* cmdBuffer = nullptr;
	CommandPool* cmdPool = nullptr;
	Util* util = nullptr;
};