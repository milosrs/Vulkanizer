#pragma once
#ifndef UTIL_H
#define UTIL_H
#endif // !UTIL_H
#include <assert.h>
#include "PLATFORM.h"
#include <iostream>
#include "BUILD_OPTIONS.h"
#include <chrono>
#include <vector>
#include <algorithm>

static auto timer = std::chrono::steady_clock();
static auto last_time = timer.now();
static uint64_t frameCounter = 0;
static uint64_t fps = 0;
static int selectedOption = -1;
static float shaderScale = 1.0f;

class CommandBufferHandler;
template <typename T> class StagingBuffer;

class Util
{
public:
	static Util& instance() {
		static Util ins;
		return ins;
	}
	~Util();

	static void printFPS();

	static void ErrorCheck(VkResult result);

	static void transitionImageLayout(VkImage, VkFormat, VkImageLayout, VkImageLayout, VkCommandPool, 
										VkQueue, VkDevice, uint32_t, VkCommandBuffer = VK_NULL_HANDLE);

	static void createImage(uint32_t, uint32_t, uint32_t, VkFormat, VkImageTiling, VkImageUsageFlags,
							VkMemoryPropertyFlags, VkImage*, VkDeviceMemory*, VkDevice, 
							VkPhysicalDeviceMemoryProperties *, VkSampleCountFlagBits = VK_SAMPLE_COUNT_1_BIT);

	static VkImageView createImageView(VkDevice, VkImage, VkFormat, VkImageAspectFlags, uint32_t);

	static void copyBufferToimage(VkBuffer, VkImage*, uint32_t, uint32_t, VkCommandPool, VkDevice, VkQueue);

	static uint32_t findMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties*, const VkMemoryRequirements*,
									const VkMemoryPropertyFlags);

	/*Used by Depth and Stencil buffers*/
	static VkFormat findSupportedFormat(VkPhysicalDevice, VkDevice, const std::vector<VkFormat> &candidates, 
										VkImageTiling tiling, VkFormatFeatureFlags flags);

	static wchar_t *convertCharArrayToLPCWSTR(const char* charArray);

	static VkSampleCountFlagBits getMultisamplingLevels(VkPhysicalDeviceProperties);
	
	/*Checks if a given Depth format has a stencil component*/
	static bool hasStencilComponent(VkFormat depthFormat);

	/*If user selects to see something that requires depth stencil, create it*/
	static bool shouldCreateDepthStencil();
	static void setOption(int option);
	static int getOption();

	static bool getScale();
	static void setScale(float scale);

	VkDevice getDevice();
private: 
	Util();
	Util(Util const&);              // Don't Implement
	void operator=(Util const&);	// Don't implement
	static Util* ins;
	
	/* Shared GPU data*/
	VkDevice device = nullptr;
	uint32_t graphicsFamilyIndex = 0;
	VkCommandPool commandPool = nullptr;
};