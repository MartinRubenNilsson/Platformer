#pragma once
#include "math/vec2.hpp"


struct Listener
{
	float2 position;
	float2 velocity;

	bool Set() const;
};