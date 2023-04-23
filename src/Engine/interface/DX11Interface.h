#pragma once


class DX11Interface
{
public:
	static struct IDXGISwapChain* swapChain;
	static struct ID3D11Device* device;
	static struct ID3D11DeviceContext* context;

	DX11Interface();
	~DX11Interface();

	bool Init(void* hWnd);
};