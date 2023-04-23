#pragma once
#include "math/Color.h"

struct ColorComponent
{
	Color value = { 0.f, 0.f, 0.f, 0.f };
};

struct Tint : ColorComponent {};
struct TintVelocity : ColorComponent {};
