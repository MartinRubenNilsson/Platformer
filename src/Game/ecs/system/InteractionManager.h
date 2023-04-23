#pragma once
#include "ecs/system/System.h"
#include <entt/entt.hpp>

class LevelManager;
class Input;


class InteractionManager : public System
{
public:
	InteractionManager(Game&);

	void Update();

private:
	void StopInteracting();

	void InteractWithDoors();
	void InteractWithText();

	LevelManager& myLevelMgr;
	Input& myInput;
};