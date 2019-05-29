#include "pch.h"
#include "RenderObject.h"
#include "DescriptorHandler.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "UniformBuffer.h"
#include "WindowController.h"
#include "IndexBuffer.h"
#include "CommandBufferSemaphoreInfo.h"
#include "Util.h"
#include "Vertices.h"
#include "Pipeline.h"
#include "Swapchain.h"
#include "MainWindow.h"
#include "Vertices.h"
#define INCLUDE_GLTFMODEL
#include "glTFModel.hpp"


RenderObject::RenderObject(std::string name)
{
	this->window = &MainWindow::getInstance();
	this->renderer = window->getRenderer();
	this->vertices = std::make_unique<Vertices>();
	this->name = name;
	this->device = renderer->getDevice();
	this->pMemprops = renderer->getPhysicalDeviceMemoryPropertiesPTR();
	this->memprops = renderer->getPhysicalDeviceMemoryProperties();
	this->window = &MainWindow::getInstance();
	this->renderer = window->getRenderer();
}


RenderObject::~RenderObject()
{
}


void RenderObject::prepareObject(VkCommandPool cmdPool, VkQueue queue)
{
	if (vertices != nullptr) {
		VkDeviceSize verticesSize = sizeof(vertices->getVertices()[0]) * vertices->getVertices().size();
		VkDeviceSize indicesSize = sizeof(vertices->getIndices()[0]) * vertices->getIndices().size();

		if (texturePaths.size() > 0 && mode > 0) {
			VkFormat imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
			
			for (const std::string path : texturePaths) {
				Texture *t = new Texture(device, pMemprops, VK_FORMAT_R8G8B8A8_UNORM, path, mode);
				t->supportsLinearBlitFormat(renderer->getGpu());
				t->beginCreatingTexture(cmdPool, queue);
				this->textures.push_back(t);
			}
		}

		this->vertexBuffer = new VertexBuffer<Vertex>(device, memprops, verticesSize);		
		this->indexBuffer = new IndexBuffer(device, memprops, indicesSize);

		float aspect = window->getSurfaceCapatibilities().currentExtent.width / (float)window->getSurfaceCapatibilities().currentExtent.height;
		float nearPlane = 0.1f;
		float farPlane = 10.0f;

		for (auto i = 0; i < window->getSwapchain()->getImageViews().size(); ++i) {
			UniformBuffer *ub = new UniformBuffer(device, memprops);
			ub->setViewData(aspect, nearPlane, farPlane);
			uniformBuffers.push_back(ub);
		}

		vertexBuffer->fillBuffer(vertices->getVertices());
		indexBuffer->fillBuffer(vertices->getIndices());

		isPrepared = true;
	}
	else {
		throw new std::runtime_error("Vertices of an object cant be null.");
	}
}


void RenderObject::setName(std::string name)
{
	this->name = name;
}


void RenderObject::setTextureParams(std::vector<std::string> texturePaths, unsigned int mode)
{
	this->texturePaths = texturePaths;
	this->mode = mode;
}


std::string RenderObject::getName()
{
	return name;
}


bool RenderObject::isObjectReadyToRender()
{
	return isPrepared && this->name != "";
}

void RenderObject::rotate(glm::vec2 mouseDelta, glm::vec3 axis)
{
	for (auto buffer : uniformBuffers) {
		buffer->rotate(mouseDelta, axis);
	}
}


IndexBuffer * RenderObject::getIndexBuffer()
{
	return indexBuffer;
}

VertexBuffer<Vertex>* RenderObject::getVertexBuffer()
{
	return this->vertexBuffer;
}

VertexBuffer<vkglTF::Vertex>* RenderObject::getGLTFVertexBuffer()
{
	return this->glTFVertexBuffer;
}


std::vector<Texture*> RenderObject::getTextures()
{
	return this->textures;
}

Vertices* RenderObject::getVertices()
{
	return this->vertices.get();
}

std::vector<UniformBuffer*> RenderObject::getUniformBuffers()
{
	return uniformBuffers;
}

std::vector<std::string> RenderObject::getTexturePaths()
{
	return this->texturePaths;
}