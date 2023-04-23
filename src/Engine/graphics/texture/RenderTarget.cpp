#include "pch.h"
#include "graphics/texture/RenderTarget.h"
#include "interface/DX11Interface.h"


// Constructor & destructor

RenderTarget::RenderTarget() = default;
RenderTarget::~RenderTarget() = default;


// Public methods

bool RenderTarget::Create(uint2 aSize)
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
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	HRESULT result = DX11Interface::device->CreateTexture2D(&desc, NULL, &myTexture2D);
	if (FAILED(result))
		return false;

	if (!CreateSRV())
		return false;

	if (!CreateRTV())
		return false;

	return true;
}

void RenderTarget::Clear(const Color& aColor)
{
	DX11Interface::context->ClearRenderTargetView(myRTV.Get(), &aColor.r);
}

void RenderTarget::SetAsTarget() const
{
	DX11Interface::context->OMSetRenderTargets(1, myRTV.GetAddressOf(), NULL);
}


// Protected methods

bool RenderTarget::CreateRTV()
{
	HRESULT result = DX11Interface::device->CreateRenderTargetView(myTexture2D.Get(), NULL, &myRTV);
	return SUCCEEDED(result);
}
