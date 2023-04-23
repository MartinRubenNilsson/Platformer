#pragma once
#include "math/vec2.hpp"
#include "graphics/texture/RenderTarget.h"


class BackBuffer : public RenderTarget
{
public:
	BackBuffer();
	~BackBuffer() override;

	bool Init();
	bool Resize(uint2 aSize);
	void Present();
};