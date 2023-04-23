#pragma once
#include "math/vec2.hpp"


struct TransitionData
{
	float progress = 0.f; // lies in interval [0.f, 1.f]
	float garbage;
	float2 direction = { 0.f, 0.f };
};