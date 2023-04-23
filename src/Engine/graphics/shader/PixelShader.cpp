#include "pch.h"
#include "graphics/shader/PixelShader.h"
#include "interface/DX11Interface.h"


// Constructor & destructor

PixelShader::PixelShader() = default;
PixelShader::~PixelShader() = default;


// Public methods

bool PixelShader::LoadCsoFile(const fs::path& aPath)
{
	if (!LoadBytecode(aPath))
		return false;

	HRESULT result = DX11Interface::device->CreatePixelShader(myBytecode.data(), myBytecode.size(), NULL, &myPixelShader);
	return SUCCEEDED(result);
}

void PixelShader::Set() const
{
	DX11Interface::context->PSSetShader(myPixelShader.Get(), NULL, 0);
}
