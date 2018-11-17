#include "pch.h"
#include "Scene.h"

Scene::Scene(MainWindow* window, Renderer* renderer, CommandBuffer* cmdBuffer, CommandPool* cmdPool)
{
	this->window = window;
	this->renderer = renderer;
	this->cmdBuffer = cmdBuffer;
	this->cmdPool = cmdPool;
	this->util = &Util::instance();
}
