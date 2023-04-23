#pragma once

enum class PhysicsState
{
	Grounded,
	Airborne,
	Climbing,
	Stunned,
	Interacting,
	Dead
};

enum class AttackState
{
	None,
	Slashing
};