#pragma once
#include "graphics/shader/Shader.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;


class PixelShader : public Shader
{
public:
	PixelShader();
	~PixelShader() override;

	bool LoadCsoFile(const fs::path&) override;
	void Set() const override;

private:
	ComPtr<struct ID3D11PixelShader> myPixelShader;
};