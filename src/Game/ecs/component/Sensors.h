#pragma once
#include "math/vec2.hpp"
#include "math/Direction.h"
#include <vector>


struct Sensor
{
	float2 pos;
	Direction dir;
	float dist; // signed
	int value;
};

using Sensors = std::vector<Sensor>;
