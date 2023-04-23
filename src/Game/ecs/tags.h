#pragma once
#include <entt/entt.hpp>

#define TAG(str) using str = entt::tag< ## #str ## _hs>;
#define CASE(str) case #str ## _hs: aHandle.emplace<entt::tag< ## #str ## _hs>>(); break;


namespace Tag
{
	using namespace entt::literals;

	TAG(Player)
	TAG(Camera)
	TAG(Text)
	TAG(UI)
	TAG(HealthBar)
	TAG(ScoreCounter)
	TAG(Destructible)
	TAG(Interactable)
	TAG(Pickup)
	TAG(Enemy)
	TAG(Particle)

	TAG(Dead)
	TAG(Dealt) // relevant for damage

	inline void Emplace(entt::handle aHandle, const char* aTag)
	{
		switch (entt::hashed_string(aTag))
		{
		CASE(Player)
		CASE(Camera)
		CASE(Text)
		CASE(UI)
		CASE(HealthBar)
		CASE(ScoreCounter)
		CASE(Destructible)
		CASE(Interactable)
		CASE(Pickup)
		CASE(Enemy)
		CASE(Particle)
		}
	}
}


#undef TAG
#undef CASE