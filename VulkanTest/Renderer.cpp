#include "pch.h"
#include "Renderer.h"
#include "MainWindow.h"

Renderer::Renderer()
{
	util = &Util::instance();
	SetupLayersAndExtensions();
	SetupDebug();
	_InitInstance();
	_InitDevice();
	InitDebug();
}


Renderer::~Renderer()
{
	DeinitDebug();
	_DeinitInstance();
	_DeinitDevice();
	delete this->window;
}

void Renderer::_InitInstance() {
	VkInstanceCreateInfo instance_create_info{};
	VkApplicationInfo application_info{};

	application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	application_info.apiVersion = VK_MAKE_VERSION(1,1,0);
	application_info.applicationVersion = VK_MAKE_VERSION(1, 1, 0);
	application_info.pApplicationName = "Hello world by Riki";

	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.pApplicationInfo = &application_info;
	instance_create_info.enabledLayerCount = instanceLayers.size();
	instance_create_info.ppEnabledLayerNames = instanceLayers.data();
	instance_create_info.enabledExtensionCount = instanceExtensions.size();
	instance_create_info.ppEnabledExtensionNames = instanceExtensions.data();
	instance_create_info.pNext = &debugCallbackCreateInfo;

	VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance);

	if (result < 0) {
		assert(0 && "Vulkan Error: Create instance failed.");
		std::exit(result);
	}
}

void Renderer::_DeinitInstance() {
	vkDestroyInstance(instance, nullptr);
}

void Renderer::_DeinitDevice() {

	if (device != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(device);
		vkDestroyDevice(device, nullptr);
	}
}

//Trazimo GPU, smestamo sve GPU u listu.
void Renderer::_InitDevice() {
	VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
	VkDeviceCreateInfo deviceCreateInfo{};
	float queuePriorities[]{1.0f};

	this->createPhysicalDevices();
	this->enumerateInstanceLayers();
	this->enumerateDeviceLayers();

	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.queueFamilyIndex = this->graphicsFamilyIndex;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	
	auto err = vkCreateDevice(this->gpu, &deviceCreateInfo, nullptr, &device);
	if (VK_SUCCESS != err) {
		assert(0 && "Vulkan Error: Device creation failed.");
		exit(1);
	}

	util->ErrorCheck(err);

	vkGetDeviceQueue(device, this->graphicsFamilyIndex, 0, &this->queue);			//Iz kog reda hocemo da fetchujemo? Mozemo imati vise queue...
}

void Renderer::createQueueFamilyProperties() {
	uint32_t familyCount = 0;
	bool foundWantedQueue = false;

	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, nullptr);	//Koliko porodica postoje u GPU?
	std::vector<VkQueueFamilyProperties> familyPropertyList(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, familyPropertyList.data());

	for (uint32_t i = 0; i < familyCount; ++i) {
		VkQueueFamilyProperties prop = familyPropertyList[i];

		if (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			foundWantedQueue = true;
			this->graphicsFamilyIndex = i;
			break;
		}
	}

	if (!foundWantedQueue) {
		assert(0 && "Vulkan Error: No GRAPHICS queue family found.");
		exit(-1);
	}
}

void Renderer::createPhysicalDevices() {
	VkPhysicalDevice ret = NULL;
	uint32_t gpuCount = 0;
	util->ErrorCheck(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr));
	std::vector<VkPhysicalDevice> gpuList(gpuCount);
	vkEnumeratePhysicalDevices(instance, &gpuCount, gpuList.data());

	gpu = gpuList[0];
	vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
	vkGetPhysicalDeviceMemoryProperties(gpu, &gpuMemoryProperties);
}

void Renderer::enumerateDeviceLayers() {
	uint32_t layerCount = 0;
	vkEnumerateDeviceLayerProperties(gpu, &layerCount, nullptr);
	std::vector<VkLayerProperties> layerProperties(layerCount);
	vkEnumerateDeviceLayerProperties(gpu, &layerCount, layerProperties.data());

	std::cout << "Device layers:  \n";
	for (auto &i : layerProperties) {
		std::cout << " " << i.layerName << "\t\t" << i.description << "\t" << i.implementationVersion << "\n";
	}

	std::cout << std::endl;
}

void Renderer::enumerateInstanceLayers() {
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> layerProperties(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

	std::cout << "Instance layers:  \n";
	for (auto &i : layerProperties) {
		std::cout << " " << i.layerName << "\t\t" << i.description << "\t" << i.implementationVersion << "\n";
	}

	std::cout << std::endl;
}

MainWindow* Renderer::createWindow(uint32_t sizeX, uint32_t sizeY, std::string windowName) {
	this->window = new MainWindow(this, sizeX, sizeY, windowName);
	return this->window;
}

bool Renderer::run() {
	if (this->window != nullptr) {
		return this->window->update();
	}

	return true;
}

const VkInstance Renderer::getInstance()
{
	return this->instance;
}

const VkPhysicalDevice Renderer::getGpu()
{
	return this->gpu;
}

#if BUILD_ENABLE_VULKAN_DEBUG

//True ili false, kako ce se layeri ponasati po nastanku greske. True - Vulkan Core ili drugi layer nece okinuti kod. False - Ide uvek do Vulkan Core-a (.
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback
(
	VkDebugReportFlagsEXT flags,					//Kojom vrstom greske/upozorenja se upravlja? (Peek definition)
	VkDebugReportObjectTypeEXT objectType,			//Tip objekta koji je proizveo gresku?
	uint64_t src_obj,								//Pokazivac na taj objekat
	size_t location,								//Bog sveti zna, ni autor ne zna
	int32_t messageCode,							//Flagovi vracaju 0, Greske vracaju 4, vrv prioritet greske
	const char* layerPrefix,						//Koji Layer je pozvao ovaj callback (String koji covek moze da procita)
	const char* msg,								//Poruka greske (Opet citljiv)
	void* userData									//Opet bog zna
) 
{
	Util& util = Util::instance();
	bool shouldShowMessage = false;
	std::cout << msg << std::endl;
	std::cout << "VulkanDebug: ";
	std::ostringstream stream;

	switch (flags) {
	case VK_DEBUG_REPORT_WARNING_BIT_EXT: stream << "WARNING bit \n" << std::endl; shouldShowMessage = true; break;
	case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT: stream << "PERFORMANCE WARNING bit \n" << std::endl; shouldShowMessage = true; break;
	case VK_DEBUG_REPORT_ERROR_BIT_EXT: stream << "ERROR bit \n" << std::endl; shouldShowMessage = true; break;
	}

	stream << "@[" << layerPrefix << "]: ";
	stream << msg << std::endl;
	std::cout << stream.str();

#ifdef _WIN32
	if(shouldShowMessage)
		MessageBox(NULL, util.convertCharArrayToLPCWSTR(stream.str().c_str()), L"Vulkan Error!", 0);
#endif // DEBUG

	return false;									//xD
}

void Renderer::SetupDebug() {
	debugCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debugCallbackCreateInfo.pfnCallback = VulkanDebugCallback;
	debugCallbackCreateInfo.flags = 
	//	VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
		 VK_DEBUG_REPORT_WARNING_BIT_EXT
		| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT 
		| VK_DEBUG_REPORT_ERROR_BIT_EXT
	//	| VK_DEBUG_REPORT_DEBUG_BIT_EXT 
		| VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT
		| 0;

	instanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
}

void Renderer::InitDebug() {
	fvkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	fvkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");

	if (nullptr == fvkCreateDebugReportCallbackEXT || nullptr == fvkDestroyDebugReportCallbackEXT) {
		assert(0 && "Vulkan Error: Error creating debug report callbacks.");
		exit(-1);
	}
	
	fvkCreateDebugReportCallbackEXT(instance, &debugCallbackCreateInfo, nullptr, &debugReportHandle);
}

void Renderer::DeinitDebug() {
	fvkDestroyDebugReportCallbackEXT(instance, debugReportHandle, nullptr);
	debugReportHandle = nullptr;
}

void Renderer::SetupLayersAndExtensions()
{
	//instanceExtensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);		Direktno koristi ekran, sto ne mozemo na PC-u i telefonu
	instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	instanceExtensions.push_back(PLATFORM_SURFACE_EXTENSION_NAME);
}

void Renderer::setupDeviceExtensions() {
	deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);		//Omogucava swapchain
}

bool isDeviceSuitable(VkPhysicalDevice device) {
	return true;
}

#else
void Renderer::SetupDebug() {};
void Renderer::DeinitDebug() {};
void Renderer::InitDebug() {};
#endif		//BUILD_ENABLE_VULKAN_DEBUG

uint32_t Renderer::getGraphicsFamilyIndex() {
	return this->graphicsFamilyIndex;
}

const VkDevice Renderer::getDevice() {
	return this->device;
}

const VkPhysicalDeviceProperties* Renderer::getGpuProperties()
{
	return &this->gpuProperties;
}

const VkDebugReportCallbackEXT Renderer::getDebugReportHandle()
{
	return this->debugReportHandle;
}

const VkDebugReportCallbackCreateInfoEXT & Renderer::getDebugCallbackCreateInfo()
{
	return debugCallbackCreateInfo;
}

const VkQueue Renderer::getQueue() {
	return this->queue;
}

const VkPhysicalDeviceMemoryProperties & Renderer::getPhysicalDeviceMemoryProperties()
{
	return this->gpuMemoryProperties;
}
