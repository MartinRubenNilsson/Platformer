#include "pch.h"
#include "ecs/system/System.h"
#include "Game.h"
#include "level/LevelManager.h"


// Constructor & destructor

System::System(Game& aGame)
	: myGame(aGame)
	, myLevel(*myGame.GetLevelMgr().GetCurrentLevel())
	, myRegistry(myLevel.GetRegistry())
{
}


// Protected methods

void System::Update()
{
	DoUpdate();
}
