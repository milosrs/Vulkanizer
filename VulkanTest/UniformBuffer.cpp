#include "pch.h"
#include "UniformBuffer.h"
#include "Util.h"

UniformBuffer::UniformBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memprops) : Buffer(device, memprops)
{
	this->size = sizeof(UBO);
	initBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, size, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

UniformBuffer::UniformBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memprops, VkDeviceSize size) : Buffer(device, memprops)
{
	this->size = size;
	initBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, size,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}


UniformBuffer::~UniformBuffer()
{
}

void UniformBuffer::update()
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//ubo.model = glm::mat4(1.0f);
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), aspect, nearPlane, farPlane);
	ubo.proj[1][1] *= -1;														//GLM je napravljen za OpenGL, Y koordinata je na dole, pa ce nam slka biti izvrnuta.

	if (Util::getOption() == 4) {
		ubo.model *= glm::scale(glm::vec3(0.1)) * glm::translate(glm::vec3(0.0, -10.0, 0.0));
	}
	else if (Util::getOption() == 5) {
		ubo.model *= glm::scale(glm::vec3(0.4));
	}

	copy();																		//Kopiramo podatke u memoriju bafera
}

void UniformBuffer::copy()
{
	void* data;

	vkMapMemory(device, bufferMemory, 0, this->size, 0, &data);
	memcpy(data, &ubo, this->size);
	vkUnmapMemory(device, bufferMemory);
}

void UniformBuffer::rotate(glm::vec2 mouseDelta)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	const float rotationSpeed = 0.5;
	glm::vec2 angle = mouseDelta * rotationSpeed * time;

	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(rotationSpeed), glm::vec3(mouseDelta.x, 0.0f, mouseDelta.y));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), aspect, nearPlane, farPlane);
	ubo.proj[1][1] *= -1;														//GLM je napravljen za OpenGL, Y koordinata je na dole, pa ce nam slka biti izvrnuta.

	if (Util::getOption() == 4) {
		ubo.model *= glm::scale(glm::vec3(0.1)) * glm::translate(glm::vec3(0.0, -10.0, 0.0));
	}
	else if (Util::getOption() == 5) {
		ubo.model *= glm::scale(glm::vec3(0.4));
	}

	copy();
}

void UniformBuffer::setViewData(float aspect, float nearPlane, float farPlane)
{
	this->aspect = aspect;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
}