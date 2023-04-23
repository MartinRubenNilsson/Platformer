#pragma once
#include <memory>
#include <vector>

class Engine;
class Clock;
class TilesetManager;
class LevelManager;
class Bank;


class Game
{
public:
	Game(Engine&);
	~Game();

	bool Init();
	void Update();

	void ImGuiEditor();

	Engine& GetEngine() { return myEngine; }
	Clock& GetClock();
	LevelManager& GetLevelMgr() { return *myLevelMgr; }

private:
	bool LoadLDtk();
	bool LoadBanks();
	void ConfigureInput();

	Engine& myEngine;
	std::unique_ptr<TilesetManager> myTilesetMgr;
	std::unique_ptr<LevelManager> myLevelMgr;
	std::vector<Bank> myBanks;
};