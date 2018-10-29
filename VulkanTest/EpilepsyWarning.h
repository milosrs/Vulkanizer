#pragma once
#include <array>
#include "PLATFORM.h"

#define PI 3.14159265389786351235489
#define CIRCLE_RAD PI*2
#define CIRCLE_THIRD CIRCLE_RAD/3.0
#define CIRCLE_THIRD_1 0
#define CIRCLE_THIRD_2 CIRCLE_THIRD
#define CIRCLE_THIRD_3 CIRCLE_THIRD * 2

static double colorRotator = 0;

class EpilepsyWarning
{
public:
	EpilepsyWarning();
	~EpilepsyWarning();

	std::array<VkClearValue, 2> getNewClearValues();

private:
	std::array<VkClearValue, 2> clearValues{};

	float getColor(float circleNo) {
		return std::sin(colorRotator + circleNo) * 0.5 + 0.5;
	}
};
