#include "pch.h"
#include "graphics/texture/Texture.h"
#include "interface/DX11Interface.h"


// Constructor & destructor

Texture::Texture() = default;
Texture::~Texture() = default;


// Public methods

bool Texture::Create(uint2 aSize)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = aSize.x;
	desc.Height = aSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	HRESULT result = DX11Interface::device->CreateTexture2D(&desc, NULL, &myTexture2D);
	if (FAILED(result))
		return false;

	if (!CreateSRV())
		return false;

	return true;
}

bool Texture::Copy(const Texture& aSource)
{
	const uint2 sourceSize = aSource.GetSize();

	if (GetSize() != sourceSize)
	{
		if (!Create(sourceSize))
			return false;
	}

	DX11Interface::context->CopyResource(myTexture2D.Get(), aSource.myTexture2D.Get());
	return true;
}

bool Texture::Copy(const Texture& aSource, const Rect& aRegion)
{
	const Rect regionRect = Rect::GetIntersection({ {}, (float2)aSource.GetSize() }, aRegion);
	const uint2 regionSize = (uint2)regionRect.GetSize();

	if (GetSize() != regionSize)
	{
		if (!Create(regionSize))
			return false;
	}

	const uint2 min = (uint2)regionRect.min;
	const uint2 max = min + regionSize;

	D3D11_BOX box = {};
	box.left = min.x;
	box.top = min.y;
	box.front = 0;
	box.right = max.x;
	box.bottom = max.y;
	box.back = 1;

	DX11Interface::context->CopySubresourceRegion(
		myTexture2D.Get(), 0, 0, 0, 0,
		aSource.myTexture2D.Get(), 0, &box
	);

	return true;
}

void Texture::SetForVS(unsigned aSlot) const
{
	DX11Interface::context->VSSetShaderResources(aSlot, 1, mySRV.GetAddressOf());
}

void Texture::SetForPS(unsigned aSlot) const
{
	DX11Interface::context->PSSetShaderResources(aSlot, 1, mySRV.GetAddressOf());
}

uint2 Texture::GetSize() const
{
	if (!myTexture2D)
		return { 0, 0 };
	D3D11_TEXTURE2D_DESC desc = {};
	myTexture2D->GetDesc(&desc);
	return { desc.Width, desc.Height };
}


// Protected methods

bool Texture::CreateSRV()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MostDetailedMip = 0;
	desc.Texture2D.MipLevels = -1;

	HRESULT result = DX11Interface::device->CreateShaderResourceView(myTexture2D.Get(), &desc, &mySRV);

	return SUCCEEDED(result);
}
