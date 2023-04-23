#pragma once
#include "math/vec2.hpp"
#include "math/Rect.h"
#include "math/Direction.h"
#include "grid/IntGrid.h"
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
#include <string>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

class TilesetManager;



class Level
{
public:
	using id_type = std::string;
	using iid_type = size_t;

	Level(TilesetManager&);

	bool LoadSuperSimple(const fs::path& aDirectory);
	void Repopulate();

	id_type GetID() const;
	iid_type GetIID() const;
	uint2 GetSize() const;
	uint2 GetGridSize() const;
	int2 GetPosition() const;
	Rect GetLocalBounds() const;
	Rect GetWorldBounds() const;

	bool HasNeighbour(const Level& aLevel, Direction* aNeighbourDirection = nullptr) const;
	
	IntGrid& GetIntGrid() { return myIntGrid; }
	entt::registry& GetRegistry() { return myRegistry; }

private:
	json myData;
	IntGrid myIntGrid;
	entt::registry myRegistry;

	TilesetManager& myTilesetMgr;
};