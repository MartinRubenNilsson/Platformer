#pragma once
#include "math/Rect.h"


struct Viewport : Rect
{
	float minDepth = 0.f;
	float maxDepth = 1.f;

	void Set() const;
};


struct ScissorRect : Rect
{
	void Set() const;
};