#pragma once

// WINDOWS
#if defined _WIN32 
// this is always defined on windows platform

#define VK_USE_PLATFORM_WIN32_KHR 1
#define PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_INCLUDE_VULKAN

#define NOMINMAX							//min() i max() fje koje su deo namespace std

#define GLM_ENABLE_EXPERIMENTAL				//ukljuci experimentalnu biblioteku
#define GLM_FORCE_RADIANS					//neka GLM koristi radijane umesto stepeni
#define GLM_FORCE_DEPTH_ZERO_TO_ONE			//depth buffer po Vulkanovoj specifikaciji [0,1] dubina. GLM koristi [-1, 1]

#define MAX_FRAMES_IN_FLIGHT 2

#include <windows.h>

#endif

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <memory>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>