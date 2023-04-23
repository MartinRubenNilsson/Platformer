#include "pch.h"
#include "graphics/state/StateManager.h"
#include "interface/DX11Interface.h"


// Constructor & destructor

StateManager::StateManager() = default;
StateManager::~StateManager() = default;


// Public methods

bool StateManager::Init()
{
	if (!CreateRasterizerStates())
		return false;

	if (!CreateSamplerStates())
		return false;

	if (!CreateBlendStates())
		return false;

	Set(RasterizerState::Default);
	Set(SamplerState::Default);
	Set(BlendState::Enabled);

	return true;
}


// Setters

void StateManager::Set(RasterizerState aState)
{
	DX11Interface::context->RSSetState(myRasterizerStates[(size_t)aState].Get());
}

void StateManager::Set(SamplerState aState)
{
	DX11Interface::context->PSSetSamplers(0, 1, mySamplerStates[(size_t)aState].GetAddressOf());
}

void StateManager::Set(BlendState aState)
{
	DX11Interface::context->OMSetBlendState(myBlendStates[(size_t)aState].Get(), NULL, 0xffffffff);
}


// Private methods

bool StateManager::Create(const D3D11_RASTERIZER_DESC* aDesc, RasterizerState aState)
{
	auto& state = myRasterizerStates[(size_t)aState];
	HRESULT result = DX11Interface::device->CreateRasterizerState(aDesc, &state);
	return SUCCEEDED(result);
}

bool StateManager::Create(const D3D11_SAMPLER_DESC* aDesc, SamplerState aState)
{
	auto& state = mySamplerStates[(size_t)aState];
	HRESULT result = DX11Interface::device->CreateSamplerState(aDesc, &state);
	return SUCCEEDED(result);
}

bool StateManager::Create(const D3D11_BLEND_DESC* aDesc, BlendState aState)
{
	auto& state = myBlendStates[(size_t)aState];
	HRESULT result = DX11Interface::device->CreateBlendState(aDesc, &state);
	return SUCCEEDED(result);
}


bool StateManager::CreateRasterizerStates()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_NONE;
	desc.ScissorEnable = TRUE;
	
	return Create(&desc, RasterizerState::Default);
}

bool StateManager::CreateSamplerStates()
{
	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MipLODBias = 0.f;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.MinLOD = 0.f;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	return Create(&desc, SamplerState::Default);
}

bool StateManager::CreateBlendStates()
{
	{
		D3D11_BLEND_DESC desc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
		if (!Create(&desc, BlendState::Disabled))
			return false;
	}

	{
		D3D11_BLEND_DESC desc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
		desc.RenderTarget[0].BlendEnable = TRUE;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

		if (!Create(&desc, BlendState::Enabled))
			return false;
	}
	
	return true;
}
