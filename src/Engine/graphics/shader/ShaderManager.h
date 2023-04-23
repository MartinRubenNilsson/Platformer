#pragma once
#include <entt/entt.hpp> // hashed_string
#include <wrl/client.h>
#include <memory> // unique_ptr
#include <string>
#include <unordered_map>
#include <filesystem>

using namespace entt::literals;
using Microsoft::WRL::ComPtr;
namespace fs = std::filesystem;


enum class ShaderType : entt::hashed_string::hash_type
{
	SpriteVS = "SpriteVS"_hs.value(),
	SpritePS = "SpritePS"_hs.value(),
	LineStripVS = "LineStripVS"_hs.value(),
	LineStripPS = "LineStripPS"_hs.value(),

	FullscreenVS = "FullscreenVS"_hs.value(),
	TransitionPushPS = "TransitionPushPS"_hs.value(),
	PostprocessPS = "PostprocessPS"_hs.value()
};


class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	bool Init();
	void Set(ShaderType);

private:
	bool LoadShaders(const fs::path& aDirectory);
	bool CreateInputLayout();

	ComPtr<struct ID3D11InputLayout> myInputLayout;
	std::unordered_map<ShaderType, std::unique_ptr<class Shader>> myShaders;
};
