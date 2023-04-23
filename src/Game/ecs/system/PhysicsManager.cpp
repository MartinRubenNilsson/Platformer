#include "pch.h"
#include "ecs/system/PhysicsManager.h"
#include "ecs/component/floats.h"
#include "ecs/component/float2s.h"
#include "ecs/component/Sensors.h"
#include "Game.h"
#include "clock/Clock.h"


// Constructor

PhysicsManager::PhysicsManager(Game& aGame)
	: System(aGame)
	, myDeltaTime(aGame.GetClock().GetDeltaTime())
{}


// Protected methods

void PhysicsManager::DoUpdate()
{
	SimulateBounceForVelocities();
	SimulateBounceForAngularVelocities();
	SimulateVelocity();
	SimulateAngularVelocity();
	SimulateGravity();
}


// Private methods

void PhysicsManager::SimulateBounceForVelocities()
{
	auto view = myRegistry.view<const Bounce, const Sensors, Velocity>();
	for (auto [entity, bounce, sensors, velocity] : view.each())
	{
		for (const Sensor& sensor : sensors)
		{
			if (sensor.dist == 0.f && dot(velocity.value, GetUnitVectorInDirection(sensor.dir)) > 0.f)
				velocity.value = bounce.value * reflect(velocity.value, GetUnitVectorInDirection(sensor.dir));
		}
	}
}

void PhysicsManager::SimulateBounceForAngularVelocities()
{
	auto view = myRegistry.view<const Bounce, const Sensors, const Velocity, AngularVelocity>();
	for (auto [entity, bounce, sensors, velocity, angularVelocity] : view.each())
	{
		for (const Sensor& sensor : sensors)
		{
			if (sensor.dist != 0.f)
				continue;

			angularVelocity.value *= bounce.value;

			if (dot(velocity.value, angularVelocity.value * GetNormalVectorToDirection(sensor.dir)) > 0.f)
				angularVelocity.value *= -1.f;
		}
	}
}

void PhysicsManager::SimulateVelocity()
{
	auto view = myRegistry.view<const Velocity, Position>();
	for (auto [entity, velocity, position] : view.each())
		position.value += velocity.value * myDeltaTime;
}

void PhysicsManager::SimulateAngularVelocity()
{
	auto view = myRegistry.view<const AngularVelocity, Rotation>();
	for (auto [entity, angularVelocity, rotation] : view.each())
		rotation.value += angularVelocity.value * myDeltaTime;
}

void PhysicsManager::SimulateGravity()
{
	auto view = myRegistry.view<const Gravity, Velocity>();
	for (auto [entity, gravity, velocity] : view.each())
		velocity.value.y += gravity.value * myDeltaTime;
}
