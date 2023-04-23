#pragma once
#include "math/vec2.hpp"
#include "math/Rect.h"
#include "math/Direction.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>

namespace fs = std::filesystem;
using json = nlohmann::json;

class Tileset;


struct EntityDef
{
	std::string id;
	std::vector<std::string> tags;
	float2 size;
	float2 pivot;
	Tileset* tileset;
	Rect tile;
	json defaultFieldDefs;
};

struct LevelNeighbour
{
	size_t iid;
	Direction dir;
};

struct LevelDef
{
	std::vector<LevelNeighbour> neighbours;
};

struct IntGridValueDef
{
	int value;
	std::string identifier;
};

struct IntGridLayerDef
{
	Tileset* tileset;
	std::vector<IntGridValueDef> values;
};


class TilesetManager;


class LDtk
{
public:
	static bool LoadProjectFile(const fs::path& aProjectFile, TilesetManager&);

	static json& GetData() { return ourData; }
	static float GetLayerDepth(const std::string& anIdentifier);
	static const EntityDef& GetEntityDef(const std::string& anIdentifier);
	static const IntGridLayerDef GetIntGridLayerDef(const std::string& anIdentifier);
	static const LevelDef& GetLevelDef(size_t iid);

private:
	static json ourData;
	static std::unordered_map<unsigned, Tileset*> ourTilesets;
	static std::unordered_map<std::string, EntityDef> ourEntityDefs;
	static std::unordered_map<std::string, float> ourLayerDepths;
	static std::unordered_map<std::string, IntGridLayerDef> ourIntGridLayerDefs;
	static std::unordered_map<size_t, LevelDef> ourLevelDefs;
};