#pragma once
#include "ecs/system/System.h"
#include <entt/entt.hpp>


class EnemyManager : public System
{
public:
	EnemyManager(class Game&);
	~EnemyManager();

	void Update();

private:
	void OnConstructTagDead(entt::registry&, entt::entity);
	void OnUpdateHealth(entt::registry&, entt::entity);

	void HandleTurnOnTouchWall();
	void HandleTurnOnTouchLedge();
	void HandleMoveAlongSurface();

	void UpdateVelocity();
	void UpdateRotationAndScale();
};