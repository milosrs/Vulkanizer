#pragma once

#include <assert.h>
#include <vulkan/vulkan.h>
#include <iostream>

class Util
{
public:
	~Util();
	void ErrorCheck(VkResult result);

	static Util& instance() {
		static Util instance;
		return instance;
	}

	Util(Util const&)			= delete;
	void operator=(Util const&) = delete;

	/* Getters and Setters */
	VkDevice getDevice();
	uint32_t getGraphicsFamilyIndex();
	VkCommandPool getCommandPool();
	void setDevice(VkDevice);
	void setGraphicsFamilyIndex(uint32_t);
	void setCommandPool(VkCommandPool);

private: 
	Util();
	Util(Util const&);              // Don't Implement
	void operator=(Util const&);	// Don't implement
	static Util* instance;

	/* Shared GPU data*/
	VkDevice device = nullptr;
	uint32_t graphicsFamilyIndex = 0;
	VkCommandPool commandPool = nullptr;
};

