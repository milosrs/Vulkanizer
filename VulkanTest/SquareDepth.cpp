#include "pch.h"
#include "SquareDepth.h"
#include "Vertices.h"
#include "MainWindow.h"
#include "Renderer.h"

SquareDepth::SquareDepth() : RenderObject()
{
	vertices->setVertices({
		{{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

		{{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	});

	this->clearValues.resize(2);
	this->clearValues[0] = { 0.2f, 0.2f, 0.2f, 1.0f };			//Background
	this->clearValues[1] = { 1.0f, 0.0f };						//Depth stencil
	vertices->setIndices({
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	});
}


SquareDepth::~SquareDepth()
{
}

void SquareDepth::draw(VkCommandBuffer cmdBuffer)
{
	vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(this->vertices->getIndices().size()), 1, 0, 0, 0);
}
