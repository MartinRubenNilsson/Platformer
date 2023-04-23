#pragma once
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;


class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	bool Create(size_t aSize);
	bool Write(const void* someData);

	void SetForVS(unsigned aSlot) const;
	void SetForPS(unsigned aSlot) const;
	void SetForCS(unsigned aSlot) const;

	size_t GetSize() const;

private:
	ComPtr<struct ID3D11Buffer> myBuffer;
};
