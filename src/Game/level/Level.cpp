#include "pch.h"
#include "level/Level.h"
#include "ecs/EntityFactory.h"
#include "ecs/ecs_funcs.h"
#include "ecs/tags.h"
#include "ecs/component/ints.h"
#include "ecs/component/floats.h"
#include "ecs/component/float2s.h"
#include "ecs/component/Rects.h"
#include "ldtk/LDtk.h"
#include "graphics/texture/Tileset.h"
#include "graphics/texture/TilesetManager.h"


// Constructor

Level::Level(TilesetManager& aTilesetMgr)
	: myTilesetMgr(aTilesetMgr)
{
}


// Public methods

bool Level::LoadSuperSimple(const fs::path& aDirectory)
{
	std::ifstream i(aDirectory / "data.json");
	if (!i.is_open())
		return false;
	i >> myData;

	assert(GetSize() % LDTK_CELL_SIZE == uint2(0, 0));

	myIntGrid.SetCellSize({ LDTK_CELL_SIZE, LDTK_CELL_SIZE });
	if (!myIntGrid.LoadCSVFile(aDirectory / "Collisions.csv"))
		return false;

	assert(GetGridSize() == myIntGrid.GetSize());

	myData["directory"] = aDirectory.string();
	myData["identifier"] = aDirectory.stem().string();

	Repopulate();

	return true;
}

void Level::Repopulate()
{
	myRegistry.clear();

	// Create backgrounds
	const fs::path directory = myData["directory"].get<fs::path>();
	for (const json& layer : myData["layers"])
	{
		const fs::path imagePath = directory / layer.get<std::string>();
		Tileset* tileset = myTilesetMgr.GetTileset(imagePath);

		if (!tileset)
			continue;

		entt::entity entity = myRegistry.create();
		myRegistry.emplace<Pivot>(entity, 0.f, 0.f);
		myRegistry.emplace<Size>(entity, (float2)tileset->GetSize());
		myRegistry.emplace<Position>(entity, 0.f, 0.f);
		myRegistry.emplace<Tileset*>(entity, tileset);
		myRegistry.emplace<Tile>(entity, Rect{ { 0.f, 0.f }, (float2)tileset->GetSize() });
		myRegistry.emplace<Depth>(entity, LDtk::GetLayerDepth(imagePath.stem().string()));
	}

	bool hasPlayer = false;

	// Create entity instances
	EntityFactory factory(myRegistry);
	for (const auto& [id, instances] : myData["entities"].items())
	{
		for (const json& instance : instances)
			factory.CreateInstance(instance);

		hasPlayer = hasPlayer || (id == "Player");
	}

	// Create special entities
	factory.CreateDefinition("Camera");
	if (hasPlayer)
	{
		factory.CreateDefinition("HealthBar");
		factory.CreateDefinition("ScoreCounter");
	}

	// Set IntGrid values
	for (auto [entity, position, value] : myRegistry.view<const Position, const IntGridValue>().each())
		myIntGrid.SetValue(position.value, value.value);
}


// Getters

std::string Level::GetID() const
{
	return myData["identifier"].get<std::string>();
}

size_t Level::GetIID() const
{
	return std::hash<std::string>{}(myData["iid"].get<std::string>());
}

uint2 Level::GetSize() const
{
	return { myData["width"].get<unsigned>(), myData["height"].get<unsigned>() };
}

uint2 Level::GetGridSize() const
{
	return GetSize() / LDTK_CELL_SIZE;
}

int2 Level::GetPosition() const
{
	return { myData["x"].get<int>(), myData["y"].get<int>() };
}

Rect Level::GetLocalBounds() const
{
	return { { 0.f, 0.f }, (float2)GetSize() };
}

Rect Level::GetWorldBounds() const
{
	const float2 position = (float2)GetPosition();
	return { position, position + GetSize() };
}

bool Level::HasNeighbour(const Level& aLevel, Direction* aNeighbourDirection) const
{
	for (const auto& neighbour : LDtk::GetLevelDef(GetIID()).neighbours)
	{
		if (neighbour.iid == aLevel.GetIID())
		{
			if (aNeighbourDirection)
				*aNeighbourDirection = neighbour.dir;
			return true;
		}
	}

	return false;
}
