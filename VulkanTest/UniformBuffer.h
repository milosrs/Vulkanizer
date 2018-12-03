#pragma once
#define GLM_FORCE_RADIANS
#include "Buffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
} typedef UBO;

class UniformBuffer : public Buffer
{
public:
	UniformBuffer(VkDevice, VkPhysicalDeviceMemoryProperties);
	~UniformBuffer();

	/*
	  Active swapchain image index
	  Aspect ratio
	  near plane
	  far plane
	*/
	void update(uint32_t, float, float, float);
	void copy(UBO*);
private:
	UBO ubo;
};

