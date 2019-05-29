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

#define USES_UNIFORM_BUFFER true

class MainWindow;
class DescriptorHandler;
class Scene;

class Renderer
{
public:
	Renderer();
	~Renderer();

	const VkInstance getInstance();
	const VkPhysicalDevice getGpu();
	const VkDevice getDevice();
	const VkPhysicalDeviceProperties* getGpuProperties();
	const VkDebugReportCallbackEXT getDebugReportHandle();
	const VkDebugReportCallbackCreateInfoEXT& getDebugCallbackCreateInfo();
	const VkQueue getQueue();

	VkDevice* getDevicePTR();
	DescriptorHandler* getDescriptorHandler();
	std::vector<VkClearValue>* getClearValues();
	VkPhysicalDeviceMemoryProperties* getPhysicalDeviceMemoryPropertiesPTR();
	VkPhysicalDeviceMemoryProperties getPhysicalDeviceMemoryProperties();
	void _DeinitInstance();
	QueueFamilyIndices* getQueueIndices();
	VkSampleCountFlagBits getMSAA();

	void initDevice();

	void recreateDescriptorHandler();
	void render(Scene);
	void createSyncObjects();
	void deleteSyncObjects();
	void createVideo();
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

	std::vector<VkSemaphore> imageAvaiableSemaphores;		//GPU-GPU sync
	std::vector<VkSemaphore> renderFinishedSemaphores;		//GPU-GPU sinhronizacija
	std::vector<VkFence> fences;							//CPU-GPU sinhronizacija
	std::vector<VkClearValue> clearValues;

	std::unique_ptr<DescriptorHandler> descriptorHandler = nullptr;
	MainWindow* window = nullptr;

	uint32_t glfwInstanceExtensionsCount = 0;
	uint32_t graphicsFamilyIndex = 0;
	const char** glfwInstanceExtensions;

	uint32_t activeImageIndex;
	size_t frameCount = 0;

	//Video data
	const std::string picturePath = "../screnshotsForVideo/";
	const std::string pictureFormat = ".jpg";
	const std::string videoFormat = ".mp4";
	std::vector<std::string> filenames;
	std::vector<std::string> picturenames;


	std::unique_ptr<QueueFamilyIndices> queueFamilyIndices = nullptr;
};

