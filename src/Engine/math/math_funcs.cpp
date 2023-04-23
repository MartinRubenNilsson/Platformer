#include "pch.h"
#include "math_funcs.h"


float PositiveRemainder(float a, float b)
{
	const float r = std::fmodf(a, b);
	return ((r >= 0.f) ? r : r + b);
}

float2 PositiveRemainder(float2 a, float2 b)
{
	return { PositiveRemainder(a.x, b.x), PositiveRemainder(a.y, b.y) };
}

int2 GetContainingCell(float2 aPosition, uint2 aCellSize)
{
	aPosition /= aCellSize;
	return { static_cast<int>(std::floorf(aPosition.x)), static_cast<int>(std::floorf(aPosition.y)) };
}

float2 GetRescaledToFitExactly(float2 aSizeToRescale, float2 aTargetSize)
{
	const float sourceAspectRatio = std::fabsf(aSizeToRescale.x / aSizeToRescale.y);
	const float targetAspectRatio = std::fabsf(aTargetSize.x / aTargetSize.y);

	if (sourceAspectRatio <= targetAspectRatio)
		return (aSizeToRescale / aSizeToRescale.y) * aTargetSize.y;
	else
		return (aSizeToRescale / aSizeToRescale.x) * aTargetSize.x;
}



float Smoothstep(float x)
{
	if (x <= 0.f)
		return 0.f;

	if (x >= 1.f)
		return 1.f;

	return x * x * (3.f - 2.f * x);
}

float Smootherstep(float x)
{
	if (x <= 0.f)
		return 0.f;

	if (x >= 1.f)
		return 1.f;

	return x * x * x * (x * (6.f * x - 15.f) + 10.f);
}
