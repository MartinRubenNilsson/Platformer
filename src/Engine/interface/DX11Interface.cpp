#include "pch.h"
#include "interface/DX11Interface.h"


// Static members

ID3D11Device* DX11Interface::device = nullptr;
ID3D11DeviceContext* DX11Interface::context = nullptr;
IDXGISwapChain* DX11Interface::swapChain = nullptr;


// Constructor & destructor

DX11Interface::DX11Interface() = default;

DX11Interface::~DX11Interface()
{
	context->Release();
	device->Release();
	swapChain->Release();
}


// Public methods

bool DX11Interface::Init(void* hWnd)
{
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = (HWND)hWnd;
	swapChainDesc.Windowed = TRUE;

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapChain,
		&device,
		NULL,
		&context
	);

	return SUCCEEDED(result);
}