#pragma once
#include "math/vec2.hpp"


struct LineStripData
{
	struct float4
	{
		float2 xy;
		float2 zw;
	};

	float4 vertices[8];
};
