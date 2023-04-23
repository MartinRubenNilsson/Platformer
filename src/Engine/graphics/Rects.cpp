#include "pch.h"
#include "graphics/Rects.h"
#include "interface/DX11Interface.h"


// Public methods

void Viewport::Set() const
{
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = min.x;
	viewport.TopLeftY = min.y;
	viewport.Width = GetWidth();
	viewport.Height = GetHeight();
	viewport.MinDepth = minDepth;
	viewport.MaxDepth = maxDepth;
	DX11Interface::context->RSSetViewports(1, &viewport);
}

void ScissorRect::Set() const
{
	D3D11_RECT scissorRect = {};
	scissorRect.left = static_cast<LONG>(min.x);
	scissorRect.top = static_cast<LONG>(min.y);
	scissorRect.right = static_cast<LONG>(max.x);
	scissorRect.bottom = static_cast<LONG>(max.y);
	DX11Interface::context->RSSetScissorRects(1, &scissorRect);
}
