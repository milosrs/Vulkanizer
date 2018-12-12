#include "pch.h"
#include "Scene.h"

Scene::Scene(MainWindow* window, Renderer* renderer)
{
	this->window = window;
	this->renderer = renderer;
	this->util = &Util::instance();

	createSyncObjects();
}

Scene::~Scene()
{
	deleteSyncObjects();
}

void Scene::createSyncObjects() {
	imageAvaiableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	fences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fcreateInfo{};
	fcreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fcreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		util->ErrorCheck(vkCreateSemaphore(renderer->getDevice(), &createInfo, nullptr, &imageAvaiableSemaphores[i]));
		util->ErrorCheck(vkCreateSemaphore(renderer->getDevice(), &createInfo, nullptr, &renderFinishedSemaphores[i]));
		util->ErrorCheck(vkCreateFence(renderer->getDevice(), &fcreateInfo, nullptr, &fences[i]));
	}
}

void Scene::deleteSyncObjects()
{
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(renderer->getDevice(), imageAvaiableSemaphores[i], nullptr);
		vkDestroySemaphore(renderer->getDevice(), renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(renderer->getDevice(), fences[i], nullptr);
	}
}
