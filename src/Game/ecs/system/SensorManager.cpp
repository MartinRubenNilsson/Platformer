#include "pch.h"
#include "ecs/system/SensorManager.h"
#include "ecs/component/Sensors.h"
#include "ecs/component/floats.h"
#include "ecs/component/float2s.h"
#include "Game.h"
#include "level/Level.h"
#include "grid/IntGrid.h"
#include "grid/IntMask.h"
#include "math/Rect.h"


// Constructor

SensorManager::SensorManager(Game& aGame)
	: System(aGame)
	, myGrid(myLevel.GetIntGrid())
{}


// Protected methods

void SensorManager::DoUpdate()
{
	PrepareSensors();
	CalculateDistances();
	ResolvePositions();
	ResolveVelocities();
}


// Private methods

void SensorManager::PrepareSensors()
{
	auto view = myRegistry.view<Sensors, const SensorsScale, const Size, const Pivot, const Position>();
	for (auto [entity, sensors, scale, size, pivot, position] : view.each())
	{
		const float2 extent = scale.value * size.value;
		const float2 center = Rect::Create(extent, pivot.value, position.value).GetCenter();

		sensors.clear();
		for (Direction dir : { Direction::Right, Direction::Left, Direction::Down, Direction::Up })
		{
			const float2 d = GetUnitVectorInDirection(dir);
			const float2 n = GetNormalVectorToDirection(dir);

			const float Cd = abs(dot(extent / 2.f, d));
			const float Cn = abs(dot(extent / 4.f, n));

			Sensor sensor = {};
			sensor.dir = dir;

			sensor.pos = center + Cd * d + Cn * n;
			sensors.push_back(sensor);

			sensor.pos = center + Cd * d - Cn * n;
			sensors.push_back(sensor);
		}
	}
}

void SensorManager::CalculateDistances()
{
	auto view = myRegistry.view<Sensors>();
	for (auto [entity, sensors] : view.each())
	{
		for (Sensor& sensor : sensors)
		{
			Hit<float> hit = {};
			myGrid.SignedRaycast({ sensor.pos, sensor.dir }, hit, Mask::solids);
			sensor.dist = hit.distance;
			sensor.value = hit.value;
		}
	}
}

void SensorManager::ResolvePositions()
{
	auto view = myRegistry.view<Sensors, Position>();
	for (auto [entity, sensors, position] : view.each())
	{
		float2 deltaPosition = { 0.f, 0.f };

		// Find deltaPosition such that when applied, all negative distances are resolved.
		for (Sensor& sensor : sensors)
		{
			const float2 dirVec = GetUnitVectorInDirection(sensor.dir);
			const float d = dot(deltaPosition, dirVec);
			const float resolvedDistance = sensor.dist - d;

			if (resolvedDistance < 0.f)
				deltaPosition += resolvedDistance * dirVec;
		}

		// Resolve sensor positions and distances.
		for (Sensor& sensor : sensors)
		{
			sensor.pos += deltaPosition;
			sensor.dist -= dot(deltaPosition, GetUnitVectorInDirection(sensor.dir));

			if (abs(sensor.dist) < 0.01f)
				sensor.dist = 0.f;
		}

		// Update entity's position.
		position.value += deltaPosition;
	}
}

void SensorManager::ResolveVelocities()
{
	auto view = myRegistry.view<Sensors, Velocity>(entt::exclude<Bounce>);
	for (auto [entity, sensors, velocity] : view.each())
	{
		for (const Sensor& sensor : sensors)
		{
			if (sensor.dist == 0.f && dot(velocity.value, GetUnitVectorInDirection(sensor.dir)) > 0.f)
				velocity.value *= abs(GetNormalVectorToDirection(sensor.dir));
		}
	}
}
