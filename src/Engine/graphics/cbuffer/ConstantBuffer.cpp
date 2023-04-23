#include "pch.h"
#include "graphics/cbuffer/ConstantBuffer.h"
#include "interface/DX11Interface.h"


// Constructor & destructor

ConstantBuffer::ConstantBuffer() = default;
ConstantBuffer::~ConstantBuffer() = default;


// Public methods

bool ConstantBuffer::Create(size_t aSize)
{
	if (aSize % 16 != 0)
		return false;

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = static_cast<UINT>(aSize);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT result = DX11Interface::device->CreateBuffer(&desc, NULL, &myBuffer);
	return SUCCEEDED(result);
}

bool ConstantBuffer::Write(const void* someData)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	HRESULT result = DX11Interface::context->Map(myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	std::memcpy(mappedResource.pData, someData, GetSize());

	DX11Interface::context->Unmap(myBuffer.Get(), 0);

	return true;
}

void ConstantBuffer::SetForVS(unsigned aSlot) const
{
	DX11Interface::context->VSSetConstantBuffers(aSlot, 1, myBuffer.GetAddressOf());
}

void ConstantBuffer::SetForPS(unsigned aSlot) const
{
	DX11Interface::context->PSSetConstantBuffers(aSlot, 1, myBuffer.GetAddressOf());
}

void ConstantBuffer::SetForCS(unsigned aSlot) const
{
	DX11Interface::context->CSSetConstantBuffers(aSlot, 1, myBuffer.GetAddressOf());
}

size_t ConstantBuffer::GetSize() const
{
	if (!myBuffer)
		return 0;
	D3D11_BUFFER_DESC desc = {};
	myBuffer->GetDesc(&desc);
	return static_cast<size_t>(desc.ByteWidth);
}
