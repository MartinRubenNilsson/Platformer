#pragma once
#include "math/vec2.hpp"
#include "math/Rect.h"
#include "math/Color.h"


struct SpriteData
{
	Rect uv;
	float2 pivot;
	float2 size;
	float2 position;
	float rotation = 0.f;
	float alpha = 1.f;
	Color tint;
};
