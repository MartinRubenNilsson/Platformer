#pragma once
#include "math/vec2.hpp"

struct Float2Component
{
	float2 value = { 0.f, 0.f };
};

struct Pivot : Float2Component {};
struct Size : Float2Component {};
struct Scale { float2 value = { 1.f, 1.f }; };
struct Position : Float2Component {};
struct LocalPosition : Float2Component {};
struct ScreenPosition : Float2Component {};
struct Velocity : Float2Component {};