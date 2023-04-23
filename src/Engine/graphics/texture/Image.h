#pragma once
#include "graphics/texture/Texture.h"
#include <filesystem>

namespace fs = std::filesystem;


class Image : public Texture
{
public:
	Image() = default;
	~Image() override = default;

	virtual bool LoadFile(const fs::path&);
};