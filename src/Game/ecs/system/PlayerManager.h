#pragma once
#include "math/Direction.h"
#include "ecs/system/System.h"
#include <entt/entt.hpp>

class Input;


class PlayerManager : public System
{
public:
	PlayerManager(Game&);
	~PlayerManager();

	void Update();

private:
	static constexpr float ourWeakGravity = 700.f;
	static constexpr float ourStrongGravity = 750.f;
	static constexpr float ourGroundAcceleration = 1200.f;

	static constexpr float ourMaxGroundSpeed = 140.f;
	static constexpr float ourMaxFallSpeed = 230.f;
	static constexpr float ourJumpSpeed = 300.f;
	static constexpr float ourClimbSpeed = 90.f;

	static constexpr float ourVerticalKnockbackSpeed = 250.f;
	static constexpr float ourLateralKnockbackSpeed = 150.f;

	void OnConstructTagDead(entt::registry&, entt::entity);
	void OnConstructTagDealt(entt::registry&, entt::entity);
	void OnUpdateHealth(entt::registry&, entt::entity);

	void Slash(Direction);

	void HandleOutOfBounds();
	void UpdateTimers();
	void SwitchPhysics();
	void HandlePhysics();
	void SwitchAttack();
	void SwitchAnimation();
	void UpdateUI();

	Input& myInput;
	const float myDeltaTime;
};