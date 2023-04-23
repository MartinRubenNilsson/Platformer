#pragma once
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;


enum class RasterizerState
{
	Default,
	Count
};

enum class SamplerState
{
	Default,
	Count
};

enum class BlendState
{
	Disabled,
	Enabled,
	Count
};


class StateManager
{
public:
	StateManager();
	~StateManager();

	bool Init();

	void Set(RasterizerState);
	void Set(SamplerState);
	void Set(BlendState);

private:
	bool Create(const struct D3D11_RASTERIZER_DESC*, RasterizerState);
	bool Create(const struct D3D11_SAMPLER_DESC*, SamplerState);
	bool Create(const struct D3D11_BLEND_DESC*, BlendState);

	bool CreateRasterizerStates();
	bool CreateSamplerStates();
	bool CreateBlendStates();

	ComPtr<struct ID3D11RasterizerState> myRasterizerStates[(size_t)RasterizerState::Count];
	ComPtr<struct ID3D11SamplerState> mySamplerStates[(size_t)SamplerState::Count];
	ComPtr<struct ID3D11BlendState> myBlendStates[(size_t)BlendState::Count];
};