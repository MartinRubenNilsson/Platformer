#include "pch.h"
#include "graphics/shader/ShaderManager.h"
#include "graphics/shader/VertexShader.h"
#include "graphics/shader/PixelShader.h"
#include "interface/DX11Interface.h"


// Constructor & destructor

ShaderManager::ShaderManager() = default;
ShaderManager::~ShaderManager() = default;


// Public methods

bool ShaderManager::Init()
{
	if (!LoadShaders(fs::current_path() / "shader"))
		return false;

	if (!CreateInputLayout())
		return false;

	return true;
}

void ShaderManager::Set(ShaderType aType)
{
	myShaders.at(aType)->Set();
}


// Private methods

bool ShaderManager::LoadShaders(const fs::path& aDirectory)
{
	for (const auto& entry : fs::directory_iterator(aDirectory))
	{
		if (!entry.is_regular_file())
			continue;

		const fs::path& path = entry.path();
		if (path.extension() != ".cso")
			continue;

		const std::string stemStr = path.stem().string();

		Shader* shader = nullptr;

		if (stemStr.ends_with("VS"))
			shader = new VertexShader();
		else if (stemStr.ends_with("PS"))
			shader = new PixelShader();
		else
			return false;

		if (!shader->LoadCsoFile(path))
		{
			delete shader;
			return false;
		}

		auto value = entt::hashed_string(stemStr.c_str()).value();
		myShaders[static_cast<ShaderType>(value)].reset(shader);
	}

	return true;
}

bool ShaderManager::CreateInputLayout()
{
	const D3D11_INPUT_ELEMENT_DESC inputElements[] =
	{
		{ "SV_VertexID", 0, DXGI_FORMAT_R8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	const std::string& shaderBytecode = myShaders.at(ShaderType::SpriteVS)->GetBytecode();

	HRESULT result = DX11Interface::device->CreateInputLayout(
		inputElements,
		(UINT)std::size(inputElements),
		shaderBytecode.data(),
		(UINT)shaderBytecode.size(),
		myInputLayout.GetAddressOf()
	);

	if (FAILED(result))
		return false;

	DX11Interface::context->IASetInputLayout(myInputLayout.Get());

	return true;
}
