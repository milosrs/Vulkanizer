#pragma once
#ifndef SQUARE_H
#define SQUARE_H
#endif
#include "PLATFORM.h"
#include "RenderObject.h"

class Vertices;

class Square : public RenderObject
{
public:
	Square();

	void draw(VkCommandBuffer cmdBuffer) override;
private:
	
};

