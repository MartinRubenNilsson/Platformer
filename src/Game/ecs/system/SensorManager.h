#pragma once
#include "ecs/system/System.h"
#include <entt/entt.hpp>

class Game;
class IntGrid;


class SensorManager : public System
{
public:
	SensorManager(Game&);

protected:
	void DoUpdate();

private:
	void PrepareSensors();
	void CalculateDistances();
	void ResolvePositions();
	void ResolveVelocities();

	IntGrid& myGrid;
};