#pragma once
#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H
#endif
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
	  Aspect ratio
	  near plane
	  far plane
	*/
	void update();
	void copy();
	void rotate(glm::vec2 mouseDelta);
	void setViewData(float, float, float);
private:
	UBO ubo = {};
	float aspect, nearPlane, farPlane;
};

