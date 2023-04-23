#pragma once
#include "math/vec2.hpp"


class PerlinNoise
{
public:
	float Get(float2 p);

private:
	static float2 GetGradient(int2 i);
	static float GetDotGradient(int2 i, float2 p);
	static float Interpolate(float x, float y, float s);
};