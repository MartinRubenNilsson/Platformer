#pragma once
#include "ecs/system/System.h"

class Game;


class PhysicsManager : public System
{
public:
	PhysicsManager(Game&);

protected:
	void DoUpdate();

private:
	void SimulateBounceForVelocities();
	void SimulateBounceForAngularVelocities();
	void SimulateVelocity();
	void SimulateAngularVelocity();
	void SimulateGravity();

	const float myDeltaTime;
};