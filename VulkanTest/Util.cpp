#include "pch.h"
#include "Util.h"

Util::Util()
{
}


Util::~Util()
{
}

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
	}
}

VkDevice Util::getDevice() {
	return device;
}

uint32_t Util::getGraphicsFamilyIndex() {
	return graphicsFamilyIndex;
}

VkCommandPool Util::getCommandPool() {
	return commandPool;
}

void Util::setDevice(VkDevice device) {
	this->device = device;
}

void Util::setGraphicsFamilyIndex(uint32_t gfi) {
	this->graphicsFamilyIndex = gfi;
}

void Util::setCommandPool(VkCommandPool pool) {
	this->commandPool = pool;
}