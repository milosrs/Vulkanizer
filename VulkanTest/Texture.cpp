#include "pch.h"
#include "Texture.h"


Texture::Texture(std::string path, unsigned int mode)
{
	pixels = stbi_load(path.c_str(), &width, &height, &channelCount, mode);
	size = width * height * mode;													//Mode je ustvari broj bajtova po pikselu

	if (!pixels) {
		throw std::runtime_error("Error opening texture file: " + path);
		exit(1);
	}
}


Texture::~Texture()
{
}

void Texture::createTextureTest(VkDevice device, VkPhysicalDeviceMemoryProperties* memprops, VkFormat imageFormat)
{
	VkImageCreateInfo info = {};
	VkMemoryRequirements imageMemoryRequirements = {};
	VkMemoryAllocateInfo allocateInfo = {};

	stagingBuffer = std::make_unique<StagingBuffer<stbi_uc*>>(device, *memprops, size);
	stagingBuffer->fillBuffer(pixels);

	stbi_image_free(pixels);

	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.imageType = VK_IMAGE_TYPE_2D;								//Kakav koordinatni sistem koristimo za texturu?
	info.extent = { width, height, 1 };								//W, H, Depth mora biti 1.
	info.mipLevels = 1;
	info.arrayLayers = 1;
	info.format = imageFormat;										//Format bi trebao da bude kao format bafera
	info.tiling = VK_IMAGE_TILING_OPTIMAL;							//Da bi smo pristupali slici iz sejdera. Linear tilingom bi mogli pristupati texelima kako hocemo.
	info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;					//Preinitialized cuva podatke o texelima posle tranzicije.
	info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;	//Sampled bitom mozemo pristupiti texturi iz sejdera.
	info.samples = VK_SAMPLE_COUNT_1_BIT;							//Multisampling
	info.flags = 0;

	util->ErrorCheck(vkCreateImage(device, &info, nullptr, &texture));
	vkGetImageMemoryRequirements(device, texture, &imageMemoryRequirements);

	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.memoryTypeIndex = util->findMemoryTypeIndex(memprops, &imageMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	allocateInfo.allocationSize = imageMemoryRequirements.size;

	util->ErrorCheck(vkAllocateMemory(device, &allocateInfo, nullptr, &textureMemory));
	util->ErrorCheck(vkBindImageMemory(device, texture, textureMemory, 0));
}