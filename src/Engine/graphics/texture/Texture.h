#pragma once
#include "math/vec2.hpp"
#include "math/Rect.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;


class Texture
{
public:
	Texture();
	virtual ~Texture();

	virtual bool Create(uint2 aSize);
	bool Copy(const Texture& aSource);
	bool Copy(const Texture& aSource, const Rect& aRegion);

	void SetForVS(unsigned aSlot) const;
	void SetForPS(unsigned aSlot) const;

	uint2 GetSize() const;

protected:
	bool CreateSRV();

	ComPtr<struct ID3D11Texture2D> myTexture2D;
	ComPtr<struct ID3D11ShaderResourceView> mySRV;
};