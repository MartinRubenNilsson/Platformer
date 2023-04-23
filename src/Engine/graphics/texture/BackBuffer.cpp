#include "pch.h"
#include "graphics/texture/BackBuffer.h"
#include "interface/DX11Interface.h"


// Constructor & destructor

BackBuffer::BackBuffer() = default;
BackBuffer::~BackBuffer() = default;


// Public methods

bool BackBuffer::Init()
{
	HRESULT result = DX11Interface::swapChain->GetBuffer(0, IID_PPV_ARGS(&myTexture2D));
	if (FAILED(result))
		return false;

	if (!CreateSRV())
		return false;

	if (!CreateRTV())
		return false;

	SetAsTarget();

	return true;
}

bool BackBuffer::Resize(uint2 aSize)
{
	DX11Interface::context->OMSetRenderTargets(0, NULL, NULL);
	*this = BackBuffer();

	HRESULT result = DX11Interface::swapChain->ResizeBuffers(2, aSize.x, aSize.y, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(result))
		return false;

	return Init();
}

void BackBuffer::Present()
{
	DX11Interface::swapChain->Present(0, 0);
}
