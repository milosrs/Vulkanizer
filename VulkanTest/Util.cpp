#include "pch.h"
#include "Util.h"

Util::Util()
{
}

void Util::printFPS()
{
	++frameCounter;

	if (last_time + std::chrono::seconds(1) < timer.now()) {
		last_time = timer.now();
		fps = frameCounter;
		frameCounter = 0;
		std::cout << "FPS: " + fps << std::endl;
	}
}

Util::~Util()
{
}

#if BUILD_ENABLE_VULKAN_RUNTIME_DEBUG

void Util::ErrorCheck(VkResult result) {
	if (result < 0) {
		std::cout << "Vulkan Error: ";
		switch (result) {
		case VK_ERROR_OUT_OF_HOST_MEMORY: std::cout << "Out of host memory" << std::endl; break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY : std::cout << "Out of device memory" << std::endl; break;
		case VK_ERROR_INITIALIZATION_FAILED : std::cout << "Initialization failed" << std::endl; break;
		case VK_ERROR_DEVICE_LOST : std::cout << "Device lost" << std::endl; break;
		case VK_ERROR_MEMORY_MAP_FAILED : std::cout << "Memory map failed" << std::endl; break;
		case VK_ERROR_LAYER_NOT_PRESENT : std::cout << "Layer not present" << std::endl; break;
		case VK_ERROR_EXTENSION_NOT_PRESENT :  std::cout << "Extension not present" << std::endl; break;
		case VK_ERROR_FEATURE_NOT_PRESENT : std::cout << "Feature not present" << std::endl; break;
		case VK_ERROR_INCOMPATIBLE_DRIVER : std::cout << "Incompatible driver" << std::endl; break;
		case VK_ERROR_TOO_MANY_OBJECTS : std::cout << "Too many objects" << std::endl; break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED : std::cout << "Format not supported" << std::endl; break;
		case VK_ERROR_FRAGMENTED_POOL : std::cout << "Fragmented pool" << std::endl; break;
		case VK_ERROR_OUT_OF_POOL_MEMORY : std::cout << "Out of pool memory" << std::endl; break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE : std::cout << "Invalid external handle" << std::endl; break;
		case VK_ERROR_SURFACE_LOST_KHR: std::cout << "Surface lost" << std::endl; break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR : std::cout << "Native window in use" << std::endl; break;
		case VK_ERROR_OUT_OF_DATE_KHR: std::cout << "Out of host date" << std::endl; break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR : std::cout << "Incompatible display" << std::endl; break;
		case VK_ERROR_VALIDATION_FAILED_EXT : std::cout << "Validation failed" << std::endl; break;
		case VK_ERROR_INVALID_SHADER_NV: std::cout << "Invalid shader" << std::endl; break;
		case VK_ERROR_FRAGMENTATION_EXT : std::cout << "Fragmentation error" << std::endl; break;
		case VK_ERROR_NOT_PERMITTED_EXT : std::cout << "Not permitted" << std::endl; break;
		}
		exit(-1);
	}
}

#else
void Util::ErrorCheck(VkResult result) {};
#endif

wchar_t* Util::convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

uint32_t Util::findMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties * memoryProps, const VkMemoryRequirements * memoryRequirements, const VkMemoryPropertyFlags memoryFlags)
{
	uint32_t ret = NULL;

	for (uint32_t i = 0; i < memoryProps->memoryTypeCount; ++i) {
		if (memoryRequirements->memoryTypeBits & (1 << i)) {
			if ((memoryProps->memoryTypes[i].propertyFlags & memoryFlags) == memoryFlags) {
				ret = i;
			}
		}
	}

	return ret;
}
