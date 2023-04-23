#pragma once
#include "graphics/shader/Shader.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;


class VertexShader : public Shader
{
public:
	VertexShader();
	~VertexShader() override;

	bool LoadCsoFile(const fs::path&) override;
	void Set() const override;

private:
	ComPtr<struct ID3D11VertexShader> myVertexShader;
};