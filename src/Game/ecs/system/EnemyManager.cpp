#include "pch.h"
#include "ecs/system/EnemyManager.h"

#include "ecs/tags.h"
#include "ecs/ecs_funcs.h"
#include "ecs/component/ints.h"
#include "ecs/component/floats.h"
#include "ecs/component/float2s.h"
#include "ecs/component/bools.h"
#include "ecs/component/Directions.h"
#include "ecs/component/Sensors.h"

#include "Game.h"
#include "clock/Clock.h"



// Constructor

EnemyManager::EnemyManager(Game& aGame)
	: System(aGame)
{
	myRegistry.on_construct<Tag::Dead>().connect<&EnemyManager::OnConstructTagDead>(this);
	myRegistry.on_update<Health>().connect<&EnemyManager::OnUpdateHealth>(this);
}

EnemyManager::~EnemyManager()
{
	myRegistry.on_construct<Tag::Dead>().disconnect<&EnemyManager::OnConstructTagDead>(this);
	myRegistry.on_update<Health>().disconnect<&EnemyManager::OnUpdateHealth>(this);
}

void EnemyManager::Update()
{
	HandleTurnOnTouchWall();
	HandleTurnOnTouchLedge();
	HandleMoveAlongSurface();
	UpdateVelocity();
	UpdateRotationAndScale();
}


// Private listeners

void EnemyManager::OnConstructTagDead(entt::registry& aRegistry, entt::entity anEntity)
{
	if (!aRegistry.all_of<Tag::Enemy>(anEntity))
		return;

	entt::handle enemy(aRegistry, anEntity);

	enemy.remove<AnimationSpeed, Shake>();

	ReplacePivotWithoutChangingBounds(enemy, { 0.5f, 0.5f });

	enemy.emplace_or_replace<Gravity>(500.f);
	enemy.emplace_or_replace<Bounce>(0.7f);

	if (!enemy.all_of<Velocity>())
		enemy.emplace<Velocity>();

	if (enemy.all_of<ColliderScale>() && !enemy.all_of<Sensors, SensorsScale>())
	{
		enemy.emplace_or_replace<Sensors>();
		enemy.emplace_or_replace<SensorsScale>(enemy.get<ColliderScale>().value);
	}

	DropCoinsUntilOutOfMoney({ aRegistry, anEntity });
	myGame.GetClock().Pause(0.1f);
}

void EnemyManager::OnUpdateHealth(entt::registry& aRegistry, entt::entity anEntity)
{
	AddHealthFlash({ aRegistry, anEntity });
}


// Private non-static methods

void EnemyManager::HandleTurnOnTouchWall()
{
	auto view = myRegistry.view<const TurnOnTouchWall, const Sensors, MoveDirection>(entt::exclude<Tag::Dead>);
	for (auto [entity, turn, sensors, moveDir] : view.each())
	{
		if (rn::any_of(sensors, [moveDir](const Sensor& s) { return s.dir == moveDir.value && s.dist == 0.f; }))
			moveDir.value = GetOppositeDirection(moveDir.value);
	}
}

void EnemyManager::HandleTurnOnTouchLedge()
{
	auto view = myRegistry.view<const TurnOnTouchLedge, const Sensors, const FloorDirection, MoveDirection>(entt::exclude<Tag::Dead>);
	for (auto [entity, turn, sensors, floorDir, moveDir] : view.each())
	{
		auto floorSensors = sensors | vw::filter([floorDir](const Sensor& s) { return s.dir == floorDir.value; });
		if (floorSensors.empty())
			continue;

		const float2 moveVec = GetUnitVectorInDirection(moveDir.value);
		const auto [backSensor, frontSensor] = 
			rn::minmax(floorSensors, {}, [moveVec](const Sensor& s) { return dot(s.pos, moveVec); });

		if (backSensor.dist == 0.f && frontSensor.dist > 0.f)
			moveDir.value = GetOppositeDirection(moveDir.value);
	}
}

void EnemyManager::HandleMoveAlongSurface()
{
	auto view = myRegistry.view<const MoveAlongSurface, const Sensors, MoveDirection, FloorDirection>(entt::exclude<Tag::Dead>);
	for (auto [entity, move, sensors, moveDir, floorDir] : view.each())
	{
		auto touchingSensors = sensors | vw::filter([](const Sensor& s) { return s.dist == 0.f; });

		// If not touching any surface, begin falling.
		if (touchingSensors.empty())
		{
			moveDir.value = floorDir.value;
			continue;
		}

		// If touching surface and falling, set surface as floor.
		if (moveDir.value == floorDir.value)
		{
			floorDir.value = touchingSensors.front().dir;
			continue;
		}

		// If moving along surface and touching wall, set wall as floor and begin moving along it.
		if (rn::any_of(touchingSensors, [moveDir](const Sensor& s) { return s.dir == moveDir.value; }))
		{
			std::swap(moveDir.value, floorDir.value);
			moveDir.value = GetOppositeDirection(moveDir.value);
		}
	}
}

void EnemyManager::UpdateVelocity()
{
	auto view = myRegistry.view<const MoveDirection, const Speed, Velocity>(entt::exclude<Tag::Dead>);
	for (auto [entity, moveDir, speed, velocity] : view.each())
		velocity.value = speed.value * GetUnitVectorInDirection(moveDir.value);
}

void EnemyManager::UpdateRotationAndScale()
{
	constexpr float pi_over_2 = 1.57079632679f;

	auto view = myRegistry.view<const MoveDirection, const FloorDirection, Rotation, Scale>(entt::exclude<Tag::Dead>);
	for (auto [entity, moveDir, floorDir, rotation, scale] : view.each())
	{
		if (int orientation = GetOrientation(moveDir.value, floorDir.value))
		{
			rotation.value = pi_over_2 * (static_cast<int>(floorDir.value) - 1);
			scale.value.x = static_cast<float>(orientation);
		}
	}
}
