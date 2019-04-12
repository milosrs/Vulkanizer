#pragma once
#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H
#endif
#include "Buffer.h"
#include <glm/glm.hpp>
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
	UniformBuffer(VkDevice, VkPhysicalDeviceMemoryProperties, VkDeviceSize);
	~UniformBuffer();

	void update();
	void copy();
	void rotate(glm::vec2 mouseDelta);
	/*
		Aspect ratio, Near, Far
	*/
	void setViewData(float, float, float);
private:
	UBO ubo = {};
	float aspect, nearPlane, farPlane;
};

