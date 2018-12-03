#include "pch.h"
#include "UniformBuffer.h"


UniformBuffer::UniformBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memprops) : Buffer(device, memprops)
{
	this->size = sizeof(UniformBufferObject);
	initBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, size, 
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		buffer, bufferMemory);
}


UniformBuffer::~UniformBuffer()
{
}

void UniformBuffer::update(uint32_t activeSwapchainImage, float aspect , float nearPlane, float farPlane)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), aspect, nearPlane, farPlane);

	ubo.proj[1][1] *= -1;														//GLM je napravljen za OpenGL, Y koordinata je na dole, pa ce nam slka biti izvrnuta.
}

void UniformBuffer::copy(UBO* ubo)
{
	void* data;
	VkDeviceSize size = sizeof(UBO);

	vkMapMemory(device, bufferMemory, 0, size, 0, &data);
	memcpy(data, ubo, size);
	vkUnmapMemory(device, bufferMemory);
}
