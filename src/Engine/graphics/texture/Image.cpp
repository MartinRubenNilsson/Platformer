#include "pch.h"
#include "graphics/texture/Image.h"
#include "interface/DX11Interface.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


// Public methods

bool Image::LoadFile(const fs::path& aPath)
{
	int width, height, channels;
	stbi_uc* imgData = stbi_load(aPath.string().c_str(), &width, &height, &channels, 0);

	if (!imgData)
		return false;

	assert(channels == 4);

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA resourceData = {};
	resourceData.pSysMem = imgData;
	resourceData.SysMemPitch = 4 * width;

	HRESULT result = DX11Interface::device->CreateTexture2D(&desc, &resourceData, &myTexture2D);
	stbi_image_free(imgData);

	if (FAILED(result))
		return false;

	if (!CreateSRV())
		return false;

	return true;
}
