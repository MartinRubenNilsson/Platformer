#include "pch.h"
#include "graphics/shader/Shader.h"


// Protected methods

bool Shader::LoadBytecode(const fs::path& aPath)
{
	if (aPath.extension() != ".cso")
		return false;
	std::ifstream i(aPath, std::ios::binary);
	if (!i.is_open())
		return false;
	myBytecode = { std::istreambuf_iterator<char>(i), std::istreambuf_iterator<char>() };
	return true;
}
