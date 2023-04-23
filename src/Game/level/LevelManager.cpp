#include "pch.h"
#include "level/LevelManager.h"


// Constructor & destructor

LevelManager::LevelManager()
	: myLevels()
	, myCurrentLevel(nullptr)
	, myNextLevel(nullptr)
{
}

LevelManager::~LevelManager() = default;


// Public methods

void LevelManager::AddLevel(Level* aLevel)
{
	myLevels.emplace_back(aLevel);
}

void LevelManager::SetNextLevel(Level* aLevel)
{
	assert(aLevel);
	myNextLevel = aLevel;
}

void LevelManager::Update()
{
	if (myNextLevel)
	{
		myCurrentLevel = myNextLevel;
		myNextLevel = nullptr;
	}
}


// Getters

Level* LevelManager::GetLevelWithID(const id_type& id)
{
	auto level = rn::find(myLevels, id, &Level::GetID);
	return level != myLevels.end() ? level->get() : nullptr;
}

Level* LevelManager::GetLevelWithIID(const idd_type& idd)
{
	auto level = rn::find(myLevels, idd, &Level::GetIID);
	return level != myLevels.end() ? level->get() : nullptr;
}

Level* LevelManager::GetContainingLevel(float2 aPositionInWorld)
{
	for (auto& level : myLevels)
	{
		if (level->GetWorldBounds().Contains(aPositionInWorld))
			return level.get();
	}

	return nullptr;
}
