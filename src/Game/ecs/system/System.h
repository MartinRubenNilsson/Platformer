#pragma once
#include <entt/entt.hpp>

class Game;
class Level;


class System
{
public:
	System(Game&);

	void Update();

protected:
	virtual void DoUpdate() {};

	Game& myGame;
	Level& myLevel;
	entt::registry& myRegistry;
};