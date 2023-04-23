#pragma once
#include "math/Direction.h"


struct PlayerTimers
{
	enum : unsigned
	{
		JumpButtonDown,
		JumpButtonHeld,
		Coyote,
		Slash,
		Invincibility,
		Count
	};

	float time[Count] = { 0.f, 0.f, 0.f };
	static constexpr float maxTime[Count] =
	{
		0.1f, // JumpButtonDown
		0.001f, // JumpButtonHeld
		0.15f, // Coyote
		0.42f, // Slash
		1.5f // Invincibility
	};

	void Repopulate(unsigned aTimer) { time[aTimer] = maxTime[aTimer]; }
};