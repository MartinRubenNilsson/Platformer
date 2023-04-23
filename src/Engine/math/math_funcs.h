#pragma once
#include "math/vec2.hpp"

float PositiveRemainder(float a, float b);
float2 PositiveRemainder(float2 a, float2 b);

int2 GetContainingCell(float2 aPosition, uint2 aCellSize); // assumes a grid with top left corner at position (0, 0)
float2 GetRescaledToFitExactly(float2 aSizeToRescale, float2 aTargetSize);

float Smoothstep(float x);
float Smootherstep(float x);