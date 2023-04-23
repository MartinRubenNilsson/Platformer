#pragma once
#include "math/vec2.hpp"
#include "math/Color.h"
#include "graphics/texture/Texture.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;


class RenderTarget : public Texture
{
public:
	RenderTarget();
	~RenderTarget() override;

	bool Create(uint2 aSize) override;
	void Clear(const Color& = {});
	void SetAsTarget() const;

protected:
	bool CreateRTV();

	ComPtr<struct ID3D11RenderTargetView> myRTV;
};