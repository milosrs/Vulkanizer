#pragma once

// WINDOWS
#if defined _WIN32 
// this is always defined on windows platform

#define VK_USE_PLATFORM_WIN32_KHR 1
#define PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#define GLFW_INCLUDE_VULKAN
#include <windows.h>

#endif

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
