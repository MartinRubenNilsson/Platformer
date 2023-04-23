#include "pch.h"
#include "graphics/shader/VertexShader.h"
#include "interface/DX11Interface.h"


// Constructor & destructor

VertexShader::VertexShader() = default;
VertexShader::~VertexShader() = default;


// Public methods

bool VertexShader::LoadCsoFile(const fs::path& aPath)
{
	if (!LoadBytecode(aPath))
		return false;

	HRESULT result = DX11Interface::device->CreateVertexShader(myBytecode.data(), myBytecode.size(), NULL, &myVertexShader);
	return SUCCEEDED(result);
}

void VertexShader::Set() const
{
	DX11Interface::context->VSSetShader(myVertexShader.Get(), NULL, 0);
}
