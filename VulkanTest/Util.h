#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assert.h>
#include "PLATFORM.h"
#include "StagingBuffer.h"
#include "CommandBufferHandler.h"
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
	static Util& instance() {
		static Util ins;
		return ins;
	}
	~Util();

	void printFPS();

	void ErrorCheck(VkResult result);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,
		VkCommandBuffer recordingBuffer);

	void createImage(uint32_t, uint32_t, VkFormat, VkImageTiling, VkImageUsageFlags, VkMemoryPropertyFlags, VkImage*,
						VkDeviceMemory*, VkDevice, VkPhysicalDeviceMemoryProperties *, VkDeviceSize, stbi_uc*, StagingBuffer<stbi_uc*>* stagingBuffer);

	VkImageView createImageView(VkDevice, VkImage, VkFormat);

	void copyBufferToimage(VkCommandBuffer, VkBuffer, VkImage, uint32_t, uint32_t);

	uint32_t findMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties*, const VkMemoryRequirements*, 
									const VkMemoryPropertyFlags);

	wchar_t *convertCharArrayToLPCWSTR(const char* charArray);
	

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

