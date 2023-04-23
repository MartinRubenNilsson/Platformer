#include "pch.h"
#include "ecs/system/PlayerManager.h"

#include "Game.h"
#include "Engine.h"
#include "clock/Clock.h"
#include "input/Input.h"

#include "level/Level.h"
#include "level/LevelManager.h"
#include "grid/IntMask.h"

#include "ecs/EntityFactory.h"
#include "ecs/tags.h"
#include "ecs/ecs_funcs.h"
#include "ecs/component/ints.h"
#include "ecs/component/floats.h"
#include "ecs/component/float2s.h"
#include "ecs/component/strings.h"
#include "ecs/component/enums.h"

#include "ecs/component/c_player.h"
#include "ecs/component/Sensors.h"

#include "math/math_funcs.h"


// Constructor & destructor

PlayerManager::PlayerManager(Game& aGame)
	: System(aGame)
	, myInput(aGame.GetEngine().GetInput())
	, myDeltaTime(aGame.GetClock().GetDeltaTime())
{
	myRegistry.on_construct<Tag::Dead>().connect<&PlayerManager::OnConstructTagDead>(this);
	myRegistry.on_construct<Tag::Dealt>().connect<&PlayerManager::OnConstructTagDealt>(this);
	myRegistry.on_update<Health>().connect<&PlayerManager::OnUpdateHealth>(this);
}

PlayerManager::~PlayerManager()
{
	myRegistry.on_construct<Tag::Dead>().disconnect<&PlayerManager::OnConstructTagDead>(this);
	myRegistry.on_construct<Tag::Dealt>().disconnect<&PlayerManager::OnConstructTagDealt>(this);
	myRegistry.on_update<Damage>().disconnect<&PlayerManager::OnConstructTagDealt>(this);
}


// Public methods

void PlayerManager::Update()
{
	HandleOutOfBounds();
	UpdateTimers();
	SwitchPhysics();
	HandlePhysics();
	SwitchAttack();
	SwitchAnimation();
	UpdateUI();
}


// Private listeners

void PlayerManager::OnConstructTagDead(entt::registry& aRegistry, entt::entity anEntity)
{
	if (!aRegistry.all_of<Tag::Player>(anEntity))
		return;

	entt::handle player(aRegistry, anEntity);

	ReplacePivotWithoutChangingBounds(player, { 0.5f, 0.5f });

	player.emplace_or_replace<Gravity>(500.f);
	player.emplace_or_replace<Bounce>(0.7f);

	CreateGameOverText(aRegistry);
	myGame.GetClock().Pause(0.2f);
}

void PlayerManager::OnUpdateHealth(entt::registry& aRegistry, entt::entity anEntity)
{
	if (!aRegistry.all_of<Tag::Player>(anEntity))
		return;

	int& newHealth = myRegistry.get<Health>(anEntity).value;
	int& oldHealth = myRegistry.get<PreviousHealth>(anEntity).value;

	newHealth = std::clamp(newHealth, 0, MAX_PLAYER_HEALTH);
	const bool lostHealth = (newHealth < oldHealth);
	oldHealth = newHealth;

	if (!lostHealth)
		return;

	AddHealthFlash({ aRegistry, anEntity });

	aRegistry.patch<PlayerTimers>(anEntity, [](PlayerTimers& timers) { timers.Repopulate(PlayerTimers::Invincibility); });

	const float2 knockbackDirection = { -1.f * aRegistry.get<Scale>(anEntity).value.x, -2.f };
	const float2 knockbackVelocity = 70.f * knockbackDirection;

	aRegistry.replace<PhysicsState>(anEntity, PhysicsState::Stunned);
	aRegistry.replace<Velocity>(anEntity, knockbackVelocity);

	AddBlinkForDuration({ aRegistry, anEntity }, PlayerTimers::maxTime[PlayerTimers::Invincibility]);
	myGame.GetClock().Pause(0.1f);
}

void PlayerManager::OnConstructTagDealt(entt::registry& aRegistry, entt::entity anEntity)
{
	if (!myRegistry.all_of<Direction>(anEntity))
		return;

	const Direction attackDir = aRegistry.get<Direction>(anEntity);
	float2 knockback = GetUnitVectorInDirection(GetOppositeDirection(attackDir));

	switch (attackDir)
	{
	case Direction::Down:
		knockback *= ourVerticalKnockbackSpeed;
		break;
	case Direction::Left:
	case Direction::Right:
		knockback *= ourLateralKnockbackSpeed;
		break;
	}

	auto view = aRegistry.view<Tag::Player, Velocity>(entt::exclude<Tag::Dead>);
	for (auto [player, velocity] : view.each())
	{
		velocity.value *= abs(GetNormalVectorToDirection(attackDir));
		velocity.value += knockback;
	}
}


// Private action methods

void PlayerManager::Slash(Direction aDirection)
{
	auto view = myRegistry.view<Tag::Player, const Scale, AttackState>();
	for (auto [player, scale, attack] : view.each())
	{
		myRegistry.replace<AttackState>(player, AttackState::Slashing);

		const float2 dirVec = GetUnitVectorInDirection(aDirection);
		const float2 localPosition = 4.f * dirVec + float2(0.f, -10.f);

		float2 slashScale = { 1.f, 1.f };
		float slashRotation = 0.f;

		if (dirVec.x != 0)
			slashScale.x = dirVec.x;
		else
		{
			slashScale.y = scale.value.x;
			slashRotation = 3.14159f / 2.f * dirVec.y;
		}

		entt::handle slash = EntityFactory(myRegistry).CreateDefinition("PlayerSlash");
		slash.emplace<Scale>(slashScale);
		slash.emplace<Rotation>(slashRotation);
		slash.emplace<Direction>(aDirection);

		AddChild({ myRegistry, player }, slash.entity(), localPosition);
	}
}


// Private update methods

void PlayerManager::HandleOutOfBounds()
{
	LevelManager& levelMgr = myGame.GetLevelMgr();

	auto view = myRegistry.view<Tag::Player, const Position>(entt::exclude<Tag::Dead>);
	for (auto [player, position] : view.each())
	{
		const float2 worldPosition = position.value + myLevel.GetPosition();

		if (myLevel.GetWorldBounds().Contains(worldPosition))
			continue;

		Level* nextLevel = levelMgr.GetContainingLevel(worldPosition);
		if (!nextLevel)
		{
			myRegistry.emplace<Tag::Dead>(player);
			continue;
		}

		CopyOverwritePlayers(nextLevel->GetRegistry(), myRegistry);

		for (auto [nextPlayer, nextPosition] : nextLevel->GetRegistry().view<Tag::Player, Position>().each())
			nextPosition.value = worldPosition - nextLevel->GetPosition();

		levelMgr.SetNextLevel(nextLevel);
		myGame.GetClock().Pause(1.f);
	}
}

void PlayerManager::UpdateTimers()
{
	auto view = myRegistry.view<PlayerTimers>();
	for (auto [entity, timers] : view.each())
	{
		for (float& t : timers.time)
			t = std::max(t - myDeltaTime, 0.f);

		if (myInput.ButtonDown("Jump"))
			timers.Repopulate(PlayerTimers::JumpButtonDown);

		if (myInput.ButtonHeld("Jump"))
			timers.Repopulate(PlayerTimers::JumpButtonHeld);

		if (myInput.ButtonDown("Attack") && timers.time[PlayerTimers::Slash] == 0.f)
			timers.Repopulate(PlayerTimers::Slash);
	}
}

void PlayerManager::SwitchPhysics()
{
	const IntGrid& aGrid = myLevel.GetIntGrid();

	auto view = myRegistry.view<PhysicsState, PlayerTimers, const Sensors, Position, Velocity>(entt::exclude<Tag::Dead>);
	for (auto [entity, physics, timers, sensors, position, velocity] : view.each())
	{
		float distToFloor = FLT_MAX;
		{
			auto floorSensors = sensors | vw::filter([](const Sensor& s) { return s.dir == Direction::Down; });
			if (!floorSensors.empty())
				distToFloor = rn::min(floorSensors, {}, &Sensor::dist).dist;
		}

		if (distToFloor <= 0.f)
			timers.Repopulate(PlayerTimers::Coyote);

		/*
		* <HELPER LAMBDAS>
		*/

		const auto TryFalling = [&physics, distToFloor]
		{
			if (distToFloor > 0.f)
			{
				physics = PhysicsState::Airborne;
				return true;
			}
			return false;
		};

		const auto TryLanding = [&physics, distToFloor]
		{
			if (distToFloor <= 0.f)
			{
				physics = PhysicsState::Grounded;
				return true;
			}
			return false;
		};

		const auto TryJumping = [this, &physics, &timers, &velocity]
		{
			if (timers.time[PlayerTimers::JumpButtonDown] > 0.f && timers.time[PlayerTimers::Coyote] > 0.f)
			{
				auto view = myRegistry.view<Tag::Player, Velocity, PhysicsState, PlayerTimers>();
				for (auto [player, velocity, physics, timers] : view.each())
				{
					velocity.value.y = -ourJumpSpeed;
					physics = PhysicsState::Airborne;
					timers.time[PlayerTimers::JumpButtonDown] = 0.f;
					timers.time[PlayerTimers::Coyote] = 0.f;
				}
				return true;
			}
			return false;
		};

		const auto TryClimbing = [this, &physics, &position, &aGrid]
		{
			constexpr float epsilon = 0.1f;
			const float verticalAxis = myInput.GetAxis("Vertical");

			if ((aGrid.GetValue(position.value + float2(0.f, epsilon), Mask::ladder) && verticalAxis > 0.f) ||
				(aGrid.GetValue(position.value - float2(0.f, epsilon), Mask::ladder) && verticalAxis < 0.f))
			{
				physics = PhysicsState::Climbing;
				position.value.y += epsilon * verticalAxis;
				return true;
			}
			return false;
		};

		/*
		* </HELPER LAMBDAS>
		*/


		switch (physics)
		{
		case PhysicsState::Grounded:
		{
			if (TryClimbing()) break;
			if (TryFalling()) break;
			if (TryJumping()) break;
			break;
		}
		case PhysicsState::Airborne:
		{
			if (TryClimbing()) break;
			if (TryJumping()) break; // if we just now walked off a ledge
			if (TryLanding()) break;
			break;
		}
		case PhysicsState::Climbing:
		{
			if (TryJumping()) break;
			if (TryLanding()) break;

			// Try fall off ladder
			if (!aGrid.GetValue(position.value, Mask::ladder))
				physics = PhysicsState::Airborne;

			break;
		}
		case PhysicsState::Stunned:
		{
			if (TryLanding()) break;
			break;
		}
		case PhysicsState::Interacting:
		{
			// what to do?
			break;
		}
		case PhysicsState::Dead:
		{
			// what to do?
			break;
		}
		}
	}
}

void PlayerManager::HandlePhysics()
{
	auto view = myRegistry.view<PhysicsState, PlayerTimers, Position, Velocity, Gravity>(entt::exclude<Tag::Dead>);
	for (auto [entity, physics, timers, position, velocity, gravity] : view.each())
	{
		/*
		* <HELPER LAMBDAS>
		*/

		const auto ControlGroundVelocity = [&velocity, this]
		{
			float deltaGroundVelocity = ourGroundAcceleration * myDeltaTime;

			if (float sign = myInput.GetAxis("Horizontal"))
				deltaGroundVelocity *= sign;
			else if (abs(velocity.value.x) > deltaGroundVelocity)
				deltaGroundVelocity *= (velocity.value.x > 0.f ? -1.f : 1.f); // opposite direction of motion
			else
				deltaGroundVelocity = -velocity.value.x; // brake immediately

			velocity.value.x = std::clamp(velocity.value.x + deltaGroundVelocity, -ourMaxGroundSpeed, ourMaxGroundSpeed);
		};

		const auto LimitFallSpeed = [&velocity]
		{
			velocity.value.y = std::min(velocity.value.y, ourMaxFallSpeed);
		};

		const auto ChooseGravityStrength = [&gravity, &velocity]
		{
			gravity.value = (velocity.value.y < 0.f ? ourStrongGravity : ourWeakGravity);
		};

		const auto MoveTowardsTileCenter = [&position, this]
		{
			constexpr uint2 cellSize = { LDTK_CELL_SIZE, LDTK_CELL_SIZE };
			const float centerX = (GetContainingCell(position.value, cellSize).x + 0.5f) * cellSize.x;
			position.value.x = std::lerp(position.value.x, centerX, 40.f * myDeltaTime);
		};

		/*
		* </HELPER LAMBDAS>
		*/


		switch (physics)
		{
		case PhysicsState::Grounded:
		{
			ControlGroundVelocity();
			velocity.value.y = 0.f;
			gravity.value = 0.f;

			break;
		}
		case PhysicsState::Airborne:
		{
			ControlGroundVelocity();
			LimitFallSpeed();
			ChooseGravityStrength();

			if (timers.time[PlayerTimers::JumpButtonHeld] == 0.f && velocity.value.y < 0.f)
				velocity.value.y *= 0.1f;

			break;
		}
		case PhysicsState::Climbing:
		{
			MoveTowardsTileCenter();
			velocity.value.x = 0.f;
			velocity.value.y = ourClimbSpeed * myInput.GetAxis("Vertical");
			gravity.value = 0.f;
			timers.Repopulate(PlayerTimers::Coyote);

			break;
		}
		case PhysicsState::Stunned:
		{
			LimitFallSpeed();
			ChooseGravityStrength();
			timers.Repopulate(PlayerTimers::Invincibility);

			break;
		}
		case PhysicsState::Interacting:
		{
			velocity.value = { 0.f, 0.f };
			break;
		}
		case PhysicsState::Dead:
		{
			LimitFallSpeed();
			ChooseGravityStrength();
			break;
		}
		}
	}
}

void PlayerManager::SwitchAttack()
{
	auto view = myRegistry.view<Tag::Player, const Scale, PlayerTimers, AttackState>(entt::exclude<Tag::Dead>);
	for (auto [entity, scale, timers, attack] : view.each())
	{
		switch (attack)
		{
		case AttackState::None:
		{
			if (timers.time[PlayerTimers::Slash] == 0.f)
				break;

			if (float verticalAxis = myInput.GetAxis("Vertical"))
				Slash(verticalAxis > 0.f ? Direction::Down : Direction::Up);
			else
				Slash(scale.value.x > 0.f ? Direction::Right : Direction::Left);

			timers.Repopulate(PlayerTimers::Slash);

			break;
		}
		case AttackState::Slashing:
		{
			if (timers.time[PlayerTimers::Slash] == 0.f)
				myRegistry.replace<AttackState>(entity, AttackState::None);

			break;
		}
		}
	}
}

void PlayerManager::SwitchAnimation()
{
	auto view = myRegistry.view<
		AnimationTag,
		AnimationTime,
		AnimationSpeed,
		const PhysicsState,
		const AttackState,
		const Velocity
	>(entt::exclude<Tag::Dead>);

	for (auto [entity, tag, time, speed, physics, attack, velocity] : view.each())
	{
		speed.value = 1.f;

		switch (physics)
		{
		case PhysicsState::Grounded:
			tag.value = "Walk";
			if (velocity.value.x == 0.f)
				time.value = 0.f;
			else
				speed.value = abs(velocity.value.x / ourMaxGroundSpeed);
			break;
		case PhysicsState::Airborne:
			if (velocity.value.y <= 0.f)
				tag.value = "Up";
			else
				tag.value = "Down";
			break;
		case PhysicsState::Climbing:
			tag.value = "Climb";
			if (velocity.value.y == 0.f)
				speed.value = 0.f;
			break;
		case PhysicsState::Stunned:
			tag.value = "Hit";
			break;
		case PhysicsState::Interacting:
			tag.value = "Back";
			break;
		}

		switch (attack)
		{
		case AttackState::Slashing:
			tag.value = "Slash";
			continue;
		}
	}
}

void PlayerManager::UpdateUI()
{
	for (auto [player, health] : myRegistry.view<Tag::Player, const Health>().each())
	{
		for (auto [healthBar, tag] : myRegistry.view<Tag::HealthBar, AnimationTag>().each())
			tag.value = std::to_string(health.value);
	}

	for (auto [player, score] : myRegistry.view<Tag::Player, const Score>().each())
	{
		for (auto [scoreCounter, text] : myRegistry.view<Tag::ScoreCounter, Text>().each())
			text.value = std::to_string(score.value);
	}
}
