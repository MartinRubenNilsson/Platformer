#include "pch.h"
#include "ecs/system/InteractionManager.h"
#include "ecs/EntityFactory.h"
#include "ecs/tags.h"
#include "ecs/ecs_funcs.h"
#include "ecs/component/bools.h"
#include "ecs/component/float2s.h"
#include "ecs/component/strings.h"
#include "ecs/component/Sensors.h"
#include "ecs/component/EntityRefs.h"
#include "ecs/component/enums.h"
#include "Game.h"
#include "Engine.h"
#include "level/Level.h"
#include "level/LevelManager.h"
#include "clock/Clock.h"
#include "input/Input.h"


// Constructor & destructor

InteractionManager::InteractionManager(Game& aGame)
	: System(aGame)
	, myLevelMgr(aGame.GetLevelMgr())
	, myInput(aGame.GetEngine().GetInput())
{
}


// Public methods

void InteractionManager::Update()
{
	auto view = myRegistry.view<Tag::Player, PhysicsState>();
	for (auto [player, physics] : view.each())
	{
		switch (physics)
		{
		case PhysicsState::Grounded:
			if (myInput.ButtonDown("Interact"))
				break; // Start checking for interactions
		case PhysicsState::Interacting:
			if (myInput.ButtonDown("Interact"))
			{
				physics = PhysicsState::Grounded;
				myGame.GetClock().Start();
				StopInteracting();
			}
		default:
			return;
		}
	}

	InteractWithDoors();
	InteractWithText();
}


// Private methods

void InteractionManager::StopInteracting()
{
	auto view = myRegistry.view<Tag::Interactable, Tag::Text>();
	myRegistry.destroy(view.begin(), view.end());
}

void InteractionManager::InteractWithDoors()
{
	for (auto [player, playerPosition]
		: myRegistry.view<Tag::Player, const Position>().each())
	{
		for (auto [entity, exit, position]
			: myRegistry.view<Tag::Interactable, const Exit, const Position>().each())
		{
			if (length(position.value - playerPosition.value) > 8.f)
				continue;

			Level* exitLevel = myLevelMgr.GetLevelWithIID(exit.levelIid);
			if (!exitLevel)
				continue;

			entt::registry& exitRegistry = exitLevel->GetRegistry();
			entt::entity exitEntity = GetEntityWithIID(exitRegistry, exit.entityIid);
			if (!exitRegistry.valid(exitEntity))
				continue;

			CopyOverwritePlayers(exitRegistry, myRegistry);

			for (auto [newPlayer, newPosition] : exitRegistry.view<Tag::Player, Position>().each())
				newPosition.value = exitRegistry.get<Position>(exitEntity).value;

			myLevelMgr.SetNextLevel(exitLevel);
		}
	}
}

void InteractionManager::InteractWithText()
{
	for (auto [player, playerPosition, physics]
		: myRegistry.view<Tag::Player, const Position, PhysicsState>().each())
	{
		for (auto [entity, interactablePosition, text]
			: myRegistry.view<Tag::Interactable, const Position, const Text>().each())
		{
			if (length(playerPosition.value - interactablePosition.value) > 8.f)
				continue;

			myGame.GetClock().Stop();
			physics = PhysicsState::Interacting;

			EntityFactory(myRegistry)
				.CreateDefinition("ScreenText")
				.replace<Text>(text.value);

			break;
		}
	}
}
