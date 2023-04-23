#include "pch.h"
#include "graphics/shader/ComputeShader.h"
#include "interface/DX11Interface.h"


// Constructor & destructor

ComputeShader::ComputeShader() = default;
ComputeShader::~ComputeShader() = default;


// Public methods

bool ComputeShader::LoadCsoFile(const fs::path& aPath)
{
	if (!LoadBytecode(aPath))
		return false;

	HRESULT result = DX11Interface::device->CreateComputeShader(myBytecode.data(), myBytecode.size(), NULL, &myComputeShader);
	return SUCCEEDED(result);
}

void ComputeShader::Set() const
{
	DX11Interface::context->CSSetShader(myComputeShader.Get(), NULL, 0);
}
