#include "pch.h"
#include "graphics/texture/TilesetManager.h"
#include "graphics/texture/Tileset.h"


// Constructor & destructor

TilesetManager::TilesetManager() = default;
TilesetManager::~TilesetManager() = default;


// Public methods

Tileset* TilesetManager::GetTileset(const fs::path& aPath)
{
	const std::string string = aPath.string();

	if (myTilesets.contains(string))
		return myTilesets.at(string).get();

	Tileset* tileset = new Tileset();
	if (!tileset->LoadFile(string))
	{
		delete tileset;
		return nullptr;
	}

	myTilesets.emplace(string, tileset);
	return tileset;
}
