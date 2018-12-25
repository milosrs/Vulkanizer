#pragma once
#ifndef UTIL_H
#define UTIL_H
#endif // !UTIL_H
#include <assert.h>
#include "PLATFORM.h"
#include <iostream>
#include "BUILD_OPTIONS.h"
#include <chrono>

static auto timer = std::chrono::steady_clock();
static auto last_time = timer.now();
static uint64_t frameCounter = 0;
static uint64_t fps = 0;

class CommandBufferHandler;
template <class T> class StagingBuffer;

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

	static void transitionImageLayout(VkImage*, VkFormat, VkImageLayout, VkImageLayout, VkCommandPool, VkQueue, VkDevice);

	static void createImage(uint32_t, uint32_t, VkFormat, VkImageTiling, VkImageUsageFlags, 
							VkMemoryPropertyFlags, VkImage*, VkDeviceMemory*, VkDevice, 
							VkPhysicalDeviceMemoryProperties *);

	static VkImageView createImageView(VkDevice, VkImage, VkFormat);

	static void copyBufferToimage(VkBuffer, VkImage*, uint32_t, uint32_t, VkCommandPool, VkDevice, VkQueue);

	static uint32_t findMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties*, const VkMemoryRequirements*,
									const VkMemoryPropertyFlags);

	static wchar_t *convertCharArrayToLPCWSTR(const char* charArray);
	

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

