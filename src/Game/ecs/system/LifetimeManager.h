#pragma once
#include "ecs/system/System.h"


class LifetimeManager : public System
{
public:
	LifetimeManager(Game&);
	~LifetimeManager();

protected:
	void DoUpdate();

private:
	static void OnDestroyDrops(entt::registry&, entt::entity);

	void UpdateLifespans();
	void DestroyParticlesOutOfBounds();

	const float myDeltaTime;
};