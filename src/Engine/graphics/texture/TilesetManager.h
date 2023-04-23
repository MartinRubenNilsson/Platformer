#pragma once
#include <unordered_map>
#include <memory>
#include <filesystem>

namespace fs = std::filesystem;

class Tileset;


class TilesetManager
{
public:
	TilesetManager();
	~TilesetManager();

	Tileset* GetTileset(const fs::path&);

private:
	std::unordered_map<std::string, std::unique_ptr<Tileset>> myTilesets;
};