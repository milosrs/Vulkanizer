#pragma once

#include <assert.h>
#include "PLATFORM.h"
#include <iostream>
#include "BUILD_OPTIONS.h"
#include <chrono>

static auto timer = std::chrono::steady_clock();
static auto last_time = timer.now();
static uint64_t frameCounter = 0;
static uint64_t fps = 0;

class Util
{
public:
	~Util();
	void ErrorCheck(VkResult result);

	static Util& instance() {
		static Util ins;
		return ins;
	}

	uint32_t findMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties*, const VkMemoryRequirements*, const VkMemoryPropertyFlags);
	wchar_t *convertCharArrayToLPCWSTR(const char* charArray);
	void printFPS();
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

