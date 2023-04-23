#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "clock/Clock.h"
#include "graphics/texture/TilesetManager.h"
#include "level/Level.h"
#include "level/LevelManager.h"
#include "audio/Bank.h"
#include "input/Input.h"
#include "ldtk/LDtk.h"
#include "ecs/system/all_systems.h"


// Constructor & destructor

Game::Game(Engine& anEngine)
	: myEngine(anEngine)
	, myTilesetMgr(new TilesetManager())
	, myLevelMgr(new LevelManager())
	, myBanks()
{
}

Game::~Game() = default;


// Public methods

bool Game::Init()
{
	if (!LoadBanks())
		return false;

	if (!LoadLDtk())
		return false;

	ConfigureInput();

	return true;
}

void Game::Update()
{
	ImGuiEditor();

	myLevelMgr->Update();
	if (!myLevelMgr->GetCurrentLevel())
		return;

	AudioManager audioMgr(*this);
	audioMgr.Update();
	{ PhysicsManager(*this).Update();	}
	{ SensorManager(*this).Update();	}
	{
		LifetimeManager lifetimeMgr(*this);
		{
			PlayerManager playerMgr(*this);
			EnemyManager enemyMgr(*this);
			playerMgr.Update();
			enemyMgr.Update();
			CollisionManager(*this).Update();
		}
		{ CameraManager(*this).Update();	}
		{
			HierarchyManager hierarchyMgr(*this);
			{ InteractionManager(*this).Update();	}
			{ TextManager(*this).Update();			}
			hierarchyMgr.Update();
		}
		lifetimeMgr.Update();
	}
	{ SpriteManager(*this).Update();	}
	{ RenderManager(*this).Update();	}
}


// Other public methods

void Game::ImGuiEditor()
{
	if (ImGui::Begin("Level Select"))
	{
		Level* activeLevel = myLevelMgr->GetCurrentLevel();

		if (ImGui::BeginListBox("Levels"))
		{
			for (Level* level : *myLevelMgr)
			{
				const bool isActive = (activeLevel == level);
				if (ImGui::Selectable(level->GetID().c_str(), isActive))
				{
					level->Repopulate();
					myLevelMgr->SetNextLevel(level);
				}
				if (isActive)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		if (activeLevel)
		{
			if (ImGui::Button("Repopulate"))
				activeLevel->Repopulate();

			ImGui::Value("Entity count", (int)activeLevel->GetRegistry().alive());
		}

		ImGui::End();
	}
}

Clock& Game::GetClock()
{
	return myEngine.GetClock();
}


// Private methods

bool Game::LoadLDtk()
{
	fs::path path = fs::current_path() / "../ldtk/platformer.ldtk";

	if (!LDtk::LoadProjectFile(path, *myTilesetMgr))
		return false;

	path.replace_extension("/simplified");

	for (json& data : LDtk::GetData()["levels"])
	{
		data["directory"] = path;

		Level* level = new Level(*myTilesetMgr);

		if (!level->LoadSuperSimple(path / data["identifier"].get<std::string>()))
		{
			delete level;
			return false;
		}

		myLevelMgr->AddLevel(level);
	}

	return true;
}

bool Game::LoadBanks()
{
	for (const auto& entry : fs::directory_iterator(fs::current_path() / "bank"))
	{
		if (!myBanks.emplace_back().LoadFile(entry))
			return false;
	}

	return true;
}

void Game::ConfigureInput()
{
	Input& input = myEngine.GetInput();

	input.CreateButton("Right", { Key::Right });
	input.CreateButton("Left", { Key::Left });
	input.CreateButton("Up", { Key::Up });
	input.CreateButton("Down", { Key::Down });

	input.CreateButton("Confirm", { Key::Z });
	input.CreateButton("Interact", { Key::Up });

	input.CreateButton("Jump", { Key::Z });
	input.CreateButton("Attack", { Key::X });

	input.CreateAxis("Horizontal", Key::Right, Key::Left);
	input.CreateAxis("Vertical", Key::Down, Key::Up);
}
