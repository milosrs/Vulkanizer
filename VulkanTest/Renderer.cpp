#include "pch.h"
#include "Renderer.h"
#include "MainWindow.h"
#include "Util.h"

VkPhysicalDevice findMostSuitableGPU(std::vector<VkPhysicalDevice>);
int ratePhysicalDevice(VkPhysicalDevice gpu);

Renderer::Renderer()
{
	SetupDebug();
	SetupLayersAndExtensions();
	_InitInstance();
	InitDebug();
}

Renderer::~Renderer()
{
	DeinitDebug();
	_DeinitInstance();
	_DeinitDevice();
}

void Renderer::_InitInstance() {
	VkInstanceCreateInfo instance_create_info{};					//Informacije o instanci aplikacije, koje slojeve koristimo, koja prosirenja...
	VkApplicationInfo application_info{};							//Informacije o samoj aplikaciji, nepotrebno ali dobro za opis

	bool areExtensionsSupported = areGLFWExtensionsSupported();

	if (areExtensionsSupported) {
		application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		application_info.apiVersion = VK_MAKE_VERSION(1, 0, 85);
		application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		application_info.pApplicationName = "Hello world by Riki";

		instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instance_create_info.pApplicationInfo = &application_info;
		instance_create_info.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
		instance_create_info.ppEnabledLayerNames = instanceLayers.data();
		instance_create_info.enabledExtensionCount = static_cast<uint32_t>(supportedExtensionProperties.size());
		instance_create_info.ppEnabledExtensionNames = supportedExtensionProperties.data();
		instance_create_info.pNext = &debugCallbackCreateInfo;

		VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance);

		if (result < 0) {
			assert(0 && "Vulkan Error: Create instance failed.");
			std::exit(result);
		}
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
void Renderer::initDevice() {
	MainWindow* window = &MainWindow::getInstance();
	this->createPhysicalDevices();

	queueFamilyIndices = std::make_unique<QueueFamilyIndices> (this->gpu, window->getSurface());
	VkDeviceCreateInfo deviceCreateInfo{};
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfo;
	bool layersAvaiable = this->enumerateInstanceLayers();
	uint32_t enabledLayersCount = 0;

	if (layersAvaiable) {
		this->enumerateDeviceLayers();

		queueFamilyIndices->createQueueCreateInfos();
		queueCreateInfo = queueFamilyIndices->getQueueCreateInfos();

		setupDeviceExtensions();

		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfo.size());
		deviceCreateInfo.pEnabledFeatures = &gpuFeatures;										//Ako smo samo uradili enumerate pa se referenciramo na stvorenu strukturu, ukljucicemo sve mogucnosti kartice 
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = instanceLayers.data();
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfo.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

		Util::ErrorCheck(vkCreateDevice(this->gpu, &deviceCreateInfo, nullptr, &device));		//Napravimo uredjaj
		queueFamilyIndices->createQueues(this->device);											//Napravimo queue za uredjaj
		this->supportedProperties.clear();														//Sprecimo memory leak
	}
	else {
		assert(0 && "VK ERROR: No layers are avaiable.");
		exit(-1);
	}
}

bool Renderer::areGLFWExtensionsSupported() {
	int supportedFound = 0;
	
	vkEnumerateInstanceExtensionProperties(nullptr, &this->extensionsCount, nullptr);		//Prvo da vidimo kolko ima podrzanih
	supportedProperties.resize(this->extensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &this->extensionsCount, supportedProperties.data());

	std::cout << "*********GLFW Extensions*********" << std::endl << std::endl;
	for (uint32_t i = 0; i < glfwInstanceExtensionsCount; i++) {
		std::cout << "Extension GLFW: " << glfwInstanceExtensions[i] << std::endl;
	}

	std::cout << "*********Vulkan Core Extensions*********" << std::endl;
	for (uint32_t i = 0; i < extensionsCount; i++) {
		std::cout << "Extension VKEnumerate: " << supportedProperties[i].extensionName << std::endl;
	}

	for (uint32_t i = 0; i < glfwInstanceExtensionsCount; i++) {
		for (uint32_t j = 0; j < extensionsCount; j++) {
			const char* extensionName = supportedProperties[j].extensionName;
			if(strcmp(extensionName, glfwInstanceExtensions[i]) == 0) {
				++supportedFound;
				this->supportedExtensionProperties.push_back(extensionName);
				break;
			}
		}
	}

	return supportedFound == this->glfwInstanceExtensionsCount;
}

void Renderer::createPhysicalDevices() {
	VkPhysicalDevice ret = NULL;
	uint32_t gpuCount = 0;
	Util::ErrorCheck(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr));

	if (gpuCount == 0) {
		assert(0 && "Vulkan Error: No GPU-s with Vulkan support present.");
		exit(-1);
	}

	std::vector<VkPhysicalDevice> gpuList(gpuCount);
	vkEnumeratePhysicalDevices(instance, &gpuCount, gpuList.data());

	gpu = findMostSuitableGPU(gpuList);
	vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
	vkGetPhysicalDeviceMemoryProperties(gpu, &gpuMemoryProperties);
	vkGetPhysicalDeviceFeatures(this->gpu, &this->gpuFeatures);				//Izlistaj mogucnost GPUA

	msaaCount = Util::getMultisamplingLevels(gpuProperties);
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

bool Renderer::enumerateInstanceLayers() {
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> layerProperties(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

	std::cout << "Instance layers:  \n";
	for (auto &i : layerProperties) {
		std::cout << " " << i.layerName << "\t\t" << i.description << "\t" << i.implementationVersion << "\n";
	}

	std::cout << std::endl;
	
	bool layerFound = false;

	for (const char* layer : instanceLayers) {	
		for (const auto& layerProp : layerProperties) {
			if (strcmp(layer, layerProp.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
	}

	return layerFound;
}

const VkInstance Renderer::getInstance()
{
	return this->instance;
}

const VkPhysicalDevice Renderer::getGpu()
{
	return this->gpu;
}

//True ili false, kako ce se layeri ponasati po nastanku greske. 
//True - Vulkan Core ili drugi layer nece okinuti kod. False - Ide uvek do Vulkan Core-a (.
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback
(
	VkDebugReportFlagsEXT flags,					//Kojom vrstom greske/upozorenja se upravlja?
	VkDebugReportObjectTypeEXT objectType,			//Tip objekta koji je proizveo gresku
	uint64_t src_obj,								//Pokazivac na taj objekat
	size_t location,								//Memorijska lokacija objekta
	int32_t messageCode,							//Kod greske
	const char* layerPrefix,						//Koji Layer je pozvao ovaj callback
	const char* msg,								//Poruka greske
	void* userData									//Loging info
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
		MessageBox(NULL, Util::convertCharArrayToLPCWSTR(stream.str().c_str()), L"Vulkan Error!", 0);
#endif // DEBUG

	return false;									//xD
}

void Renderer::SetupDebug() {
	debugCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debugCallbackCreateInfo.pfnCallback = VulkanDebugCallback;
	debugCallbackCreateInfo.flags =
		  VK_DEBUG_REPORT_WARNING_BIT_EXT
		| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
		| VK_DEBUG_REPORT_ERROR_BIT_EXT
		| 0;

	instanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	supportedExtensionProperties.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
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
	glfwInit();															//Poziva linker i ucitava vulkan-1.dll

	if (glfwVulkanSupported()) {
		glfwInstanceExtensions = glfwGetRequiredInstanceExtensions(&this->glfwInstanceExtensionsCount);
	}
	else {
		//instanceExtensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);		Direktno koristi ekran, sto ne mozemo na PC-u i telefonu
		instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		instanceExtensions.push_back(PLATFORM_SURFACE_EXTENSION_NAME);
	}
}

void Renderer::setupDeviceExtensions() {
	deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);		//Omogucava swapchain, treba da vidimo da li je podrzan
	uint32_t extensionsCount = 0;
	int supported = 0;

	vkEnumerateDeviceExtensionProperties(this->gpu, nullptr, &extensionsCount, nullptr);
	std::vector<VkExtensionProperties> allSupportedExtensions(extensionsCount);
	vkEnumerateDeviceExtensionProperties(this->gpu, nullptr, &extensionsCount, allSupportedExtensions.data());

	std::cout << "*****SUPPORTED DEVICE EXTENSIONS****" << std::endl;
	for (const VkExtensionProperties& extension : allSupportedExtensions) {
		std::cout << extension.extensionName << std::endl;
		if (strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, extension.extensionName) == 0 && supported < deviceExtensions.size()) {
			supported++;
		}
	}

	if (supported != deviceExtensions.size()) {
		std::cout << "Vulkan Error: Requested device extensions not supported." << std::endl;
		exit(-1);
	}
}

VkPhysicalDevice findMostSuitableGPU(std::vector<VkPhysicalDevice> gpuList) {
	VkPhysicalDevice ret = VK_NULL_HANDLE;
	int maxScore = 0;
	std::map<int, VkPhysicalDevice> candidates;

	for (const auto& gpu : gpuList) {
		int score = ratePhysicalDevice(gpu);
		candidates.insert(std::pair<int, VkPhysicalDevice>(score, gpu));
	}

	for (auto i = candidates.begin(); i != candidates.end(); i++) {
		if (i == candidates.begin()) {
			maxScore = i->first;
			ret = i->second;
		}
		else if (i->first > maxScore) {
			maxScore = i->first;
			ret = i->second;
		}
	}

	return ret;
}

int ratePhysicalDevice(VkPhysicalDevice gpu) {
	int score = 0;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;

	vkGetPhysicalDeviceFeatures(gpu, &features);
	vkGetPhysicalDeviceProperties(gpu, &properties);

	if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	score += properties.limits.maxImageDimension2D;
	score += properties.limits.maxComputeSharedMemorySize;
	score += properties.limits.maxGeometryShaderInvocations;
	score += features.multiViewport ? 200 : 0;
	score += features.tessellationShader ? 500 : 0;
	score += features.samplerAnisotropy ? -3500 : 0;

	if (!features.geometryShader) {
		score = 0;
	}

	return score;
}

const VkDevice Renderer::getDevice() {
	return this->device;
}

VkDevice * Renderer::getDevicePTR()
{
	return &this->device;
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

VkPhysicalDeviceMemoryProperties* Renderer::getPhysicalDeviceMemoryPropertiesPTR()
{
	return &this->gpuMemoryProperties;
}

VkPhysicalDeviceMemoryProperties Renderer::getPhysicalDeviceMemoryProperties() {
	return this->gpuMemoryProperties;
}

QueueFamilyIndices* Renderer::getQueueIndices()
{
	return this->queueFamilyIndices.get();
}

VkSampleCountFlagBits Renderer::getMSAA()
{
	return msaaCount;
}
