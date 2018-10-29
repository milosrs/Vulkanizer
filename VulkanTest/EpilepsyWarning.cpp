#include "pch.h"
#include "EpilepsyWarning.h"

EpilepsyWarning::EpilepsyWarning()
{
	this->clearValues = std::array<VkClearValue, 2> {};
}

std::array<VkClearValue, 2> EpilepsyWarning::getNewClearValues() {
	colorRotator += 0.000000001;

	clearValues[0].depthStencil.depth = 0.0f;
	clearValues[0].depthStencil.stencil = 0;
	clearValues[1].color.float32[0] = getColor(CIRCLE_THIRD_1);			//Ima veze sa surfaceom. InitSurface linija 80.	R
	clearValues[1].color.float32[1] = getColor(CIRCLE_THIRD_2);			//Ima veze sa surfaceom. InitSurface linija 80. G
	clearValues[1].color.float32[2] = getColor(CIRCLE_THIRD_3);			//Ima veze sa surfaceom. InitSurface linija 80. B
	clearValues[1].color.float32[3] = 0.0f;								//Ima veze sa surfaceom. InitSurface linija 80. A
}

EpilepsyWarning::~EpilepsyWarning()
{
}
