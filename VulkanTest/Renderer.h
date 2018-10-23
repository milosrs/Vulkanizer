#pragma once
#ifndef RENDERER_H
#define RENDERER_H
#endif // !RENDERER_H

#include "PLATFORM.h"
#include <vector>
#include <cstdlib>
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "Util.h"
#include "BUILD_OPTIONS.h"

class MainWindow;

class Renderer
{
public:
	Renderer();
	~Renderer();

	uint32_t getGraphicsFamilyIndex();

	MainWindow* createWindow(uint32_t, uint32_t, std::string);
	bool run();

	const VkInstance getInstance();
	const VkPhysicalDevice getGpu();
	const VkDevice getDevice();
	const VkPhysicalDeviceProperties& getGpuProperties();
	const VkDebugReportCallbackEXT getDebugReportHandle();
	const VkDebugReportCallbackCreateInfoEXT& getDebugCallbackCreateInfo();
	const VkQueue getQueue();

private:
	void _InitInstance();
	void _DeinitInstance();

	void _InitDevice();
	void _DeinitDevice();

	void SetupDebug();
	void InitDebug();
	void DeinitDebug();
	void SetupLayersAndExtensions();

	void enumerateInstanceLayers();
	void enumerateDeviceLayers();
	void createQueueFamilyProperties();
	void createPhysicalDevices();

	VkInstance instance = nullptr;
	VkPhysicalDevice gpu = nullptr;
	VkDevice device = nullptr;
	VkPhysicalDeviceProperties gpuProperties = {};
	VkDebugReportCallbackEXT debugReportHandle = nullptr;
	VkDebugReportCallbackCreateInfoEXT debugCallbackCreateInfo = {};
	VkQueue	queue = nullptr;

	

	PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallbackEXT = nullptr;
	PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallbackEXT = nullptr;

	std::vector<const char*> instanceLayers;
	std::vector<const char*> instanceExtensions;


	MainWindow* window = NULL;
	uint32_t graphicsFamilyIndex = 0;
};

