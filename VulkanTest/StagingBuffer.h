#pragma once
#include "Buffer.h"
#include <vector>

template<class T>
class StagingBuffer : public Buffer
{
public:
	StagingBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memprops, VkDeviceSize size) : Buffer(device, memprops) {
		this->size = size;

		this->initBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			buffer, bufferMemory);

	};

	~StagingBuffer() {};

	void fillBuffer(std::vector<T> inputData) {
		void* data;
		vkMapMemory(device, bufferMemory, 0, size, 0, &data);
		memcpy(data, inputData.data(), (size_t)size);
		vkUnmapMemory(device, bufferMemory);
	};
private:
	VkDeviceSize size = VK_NULL_HANDLE;
};

