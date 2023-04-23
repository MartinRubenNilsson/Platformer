#pragma once
#include "level/Level.h"
#include <vector>
#include <memory>

class Level;


class LevelManager
{
public:
	using id_type = Level::id_type;
	using idd_type = Level::iid_type;

	LevelManager();
	~LevelManager();

	void AddLevel(Level*);
	void SetNextLevel(Level*);
	void Update();

	Level* GetCurrentLevel() { return myCurrentLevel; }
	Level* GetNextLevel() { return myNextLevel; }
	Level* GetLevelWithID(const id_type&);
	Level* GetLevelWithIID(const idd_type&);
	Level* GetContainingLevel(float2 aPositionInWorld);

	// todo: add getneighbours

private:
	std::vector<std::unique_ptr<Level>> myLevels;
	Level* myCurrentLevel;
	Level* myNextLevel;

	using base_iterator = decltype(myLevels)::iterator;

public:
	struct iterator : base_iterator
	{
		iterator(base_iterator itr) : base_iterator(itr) {}
		Level* operator*() { return base_iterator::operator*().get(); }
	};

	iterator begin() { return myLevels.begin(); }
	iterator end() { return myLevels.end(); }
};