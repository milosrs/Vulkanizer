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
#include "QueueFamilyIndices.h"
#include <cstring>
#include <iostream>
#include <map>

class MainWindow;

class Renderer
{
public:
	Renderer();
	~Renderer();

	MainWindow* createWindow(uint32_t, uint32_t, std::string);
	bool run();

	const VkInstance getInstance();
	const VkPhysicalDevice getGpu();
	const VkDevice getDevice();
	const VkPhysicalDeviceProperties* getGpuProperties();
	const VkDebugReportCallbackEXT getDebugReportHandle();
	const VkDebugReportCallbackCreateInfoEXT& getDebugCallbackCreateInfo();
	const VkQueue getQueue();
	const VkPhysicalDeviceMemoryProperties& getPhysicalDeviceMemoryProperties();
	void _DeinitInstance();
	QueueFamilyIndices* getQueueIndices();

	void continueInitialization();
private:
	void _InitInstance();

	void _InitDevice();
	void _DeinitDevice();

	void SetupDebug();
	void InitDebug();
	void DeinitDebug();
	void SetupLayersAndExtensions();
	void setupDeviceExtensions();

	bool enumerateInstanceLayers();
	void enumerateDeviceLayers();
	void createPhysicalDevices();

	bool areGLFWExtensionsSupported();

	VkInstance instance = nullptr;
	VkPhysicalDevice gpu = VK_NULL_HANDLE;
	VkDevice device = nullptr;
	VkPhysicalDeviceProperties gpuProperties = {};
	VkDebugReportCallbackEXT debugReportHandle = nullptr;
	VkDebugReportCallbackCreateInfoEXT debugCallbackCreateInfo = {};
	VkQueue	queue = nullptr;
	VkPhysicalDeviceMemoryProperties gpuMemoryProperties = {};
	VkPhysicalDeviceFeatures gpuFeatures = {};
	

	PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallbackEXT = nullptr;
	PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallbackEXT = nullptr;

	uint32_t extensionsCount = 0;
	std::vector<VkExtensionProperties> supportedExtensionProperties;

	std::vector<const char*> instanceLayers;
	std::vector<const char*> instanceExtensions;
	std::vector<const char*> deviceExtensions;

	uint32_t glfwInstanceExtensionsCount = 0;
	const char** glfwInstanceExtensions;


	Util* util = nullptr;
	MainWindow* window = nullptr;
	uint32_t graphicsFamilyIndex = 0;
	QueueFamilyIndices* queueFamilyIndices = nullptr;
};

