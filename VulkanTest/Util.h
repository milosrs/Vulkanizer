#pragma once

#include <assert.h>
#include "PLATFORM.h"
#include <iostream>
#include "BUILD_OPTIONS.h"

class Util
{
public:
	~Util();
	void ErrorCheck(VkResult result);

	static Util& instance() {
		static Util ins;
		return ins;
	}

	/* Getters and Setters */
	VkDevice getDevice();
	uint32_t getGraphicsFamilyIndex();
	VkCommandPool getCommandPool();

	void setDevice(VkDevice);
	void setGraphicsFamilyIndex(uint32_t);
	void setCommandPool(VkCommandPool);
	wchar_t *convertCharArrayToLPCWSTR(const char* charArray);

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

