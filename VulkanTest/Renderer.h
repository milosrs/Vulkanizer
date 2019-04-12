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
#include "BUILD_OPTIONS.h"
#include <cstring>
#include <iostream>
#include <map>

#include "QueueFamilyIndices.h"

class MainWindow;

class Renderer
{
public:
	Renderer();
	~Renderer();

	const VkInstance getInstance();
	const VkPhysicalDevice getGpu();
	const VkDevice getDevice();
	VkDevice* getDevicePTR();
	const VkPhysicalDeviceProperties* getGpuProperties();
	const VkDebugReportCallbackEXT getDebugReportHandle();
	const VkDebugReportCallbackCreateInfoEXT& getDebugCallbackCreateInfo();
	const VkQueue getQueue();
	VkPhysicalDeviceMemoryProperties* getPhysicalDeviceMemoryPropertiesPTR();
	VkPhysicalDeviceMemoryProperties getPhysicalDeviceMemoryProperties();
	void _DeinitInstance();
	QueueFamilyIndices* getQueueIndices();
	VkSampleCountFlagBits getMSAA();

	void initDevice();
private:
	void _InitInstance();

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

	VkInstance instance = VK_NULL_HANDLE;
	VkPhysicalDevice gpu = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties gpuProperties = {};
	VkDebugReportCallbackEXT debugReportHandle = VK_NULL_HANDLE;
	VkDebugReportCallbackCreateInfoEXT debugCallbackCreateInfo = {};
	VkQueue	queue = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties gpuMemoryProperties = {};
	VkPhysicalDeviceFeatures gpuFeatures = {};
	VkSampleCountFlagBits msaaCount = VK_SAMPLE_COUNT_1_BIT;
	
	PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallbackEXT = VK_NULL_HANDLE;
	PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallbackEXT = VK_NULL_HANDLE;

	uint32_t extensionsCount = 0;
	std::vector<const char*> supportedExtensionProperties;
	std::vector<VkExtensionProperties> supportedProperties;

	std::vector<const char*> instanceLayers;
	std::vector<const char*> instanceExtensions;
	std::vector<const char*> deviceExtensions;

	uint32_t glfwInstanceExtensionsCount = 0;
	uint32_t graphicsFamilyIndex = 0;
	const char** glfwInstanceExtensions;

	std::unique_ptr<QueueFamilyIndices> queueFamilyIndices = nullptr;
};

