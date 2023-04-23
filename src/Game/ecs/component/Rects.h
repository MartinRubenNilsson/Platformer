#pragma once
#include "math/Rect.h"

struct RectComponent
{
	Rect value = { { 0.f, 0.f }, { 1.f, 1.f } };
};

struct Tile : RectComponent {}; // pixels, not UV
struct Collider : RectComponent {};