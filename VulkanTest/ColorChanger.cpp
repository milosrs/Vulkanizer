#include "pch.h"
#include "ColorChanger.h"

ColorChanger::ColorChanger() : RenderObject()
{
	this->clearValues = std::array<VkClearValue, 2> {};
}

std::array<VkClearValue, 2> ColorChanger::getNewClearValues() {
	colorRotator += 0.000000001;

	clearValues[0].depthStencil.depth = 0.0f;
	clearValues[0].depthStencil.stencil = 0;
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };

	clearValues[1].depthStencil.depth = 0.0f;
	clearValues[1].depthStencil.stencil = 0.0f;
	clearValues[1].color = { getColor(CIRCLE_THIRD_1), getColor(CIRCLE_THIRD_2), getColor(CIRCLE_THIRD_3), 0.0f };

	return clearValues;
}

float ColorChanger::getColor(float circleNo) {
	return std::sin(colorRotator + circleNo) * 0.5 + 0.5;
}