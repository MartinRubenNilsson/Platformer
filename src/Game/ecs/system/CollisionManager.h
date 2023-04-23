#pragma once
#include "ecs/system/System.h"

class Game;
class IntGrid;


class CollisionManager : public System
{
public:
	CollisionManager(Game&);

protected:
	void DoUpdate();

private:
	void PrepareColliders();

	void PlayerVsHealthPickups();
	void PlayerVsScorePickups();
	void PlayerVsEnemies();

	void DamageVsEnemies();
	void DamageVsDestructibles();

	IntGrid& myGrid;
};