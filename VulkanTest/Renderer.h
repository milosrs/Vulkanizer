#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdlib>
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "Util.h"

#ifdef _WIN32
#include <Windows.h>
#endif

class Renderer
{
public:
	Renderer();
	~Renderer();

	uint32_t getGraphicsFamilyIndex();
	VkDevice getDevice();

	VkQueue getQueue();

private:
	void _InitInstance();
	void _DeinitInstance();

	void _InitDevice();
	void _DeinitDevice();

	void SetupDebug();
	void InitDebug();
	void DeinitDebug();

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

	uint32_t graphicsFamilyIndex = 0;
};

