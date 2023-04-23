#pragma once
#include <string>
#include <filesystem>

namespace fs = std::filesystem;


class Shader
{
public:
	virtual ~Shader() = default;

	virtual bool LoadCsoFile(const fs::path&) = 0;
	virtual void Set() const = 0;

	const std::string& GetBytecode() const { return myBytecode; }

protected:
	bool LoadBytecode(const fs::path&);
	
	std::string myBytecode;
};