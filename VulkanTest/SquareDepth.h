#pragma once
#ifndef SQUARE_H
#define SQUARE_H
#endif
#include "PLATFORM.h"
#include "RenderObject.h"

class Vertices;
class MainWindow;
class Renderer;

class SquareDepth : public RenderObject
{
public:
	SquareDepth();
	~SquareDepth();

	void draw(VkCommandBuffer cmdBuffer) override;
};

