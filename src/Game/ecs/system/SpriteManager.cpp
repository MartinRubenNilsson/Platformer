#include "pch.h"
#include "ecs/system/SpriteManager.h"
#include "ecs/component/bools.h"
#include "ecs/component/floats.h"
#include "ecs/component/float2s.h"
#include "ecs/component/strings.h"
#include "ecs/component/Colors.h"
#include "ecs/component/Rects.h"
#include "ecs/component/enums.h"
#include "graphics/texture/Tileset.h"
#include "Game.h"
#include "clock/Clock.h"


// Constructor

SpriteManager::SpriteManager(Game& aGame)
	: System(aGame)
	, myDeltaTime(aGame.GetClock().GetDeltaTime())
{}


// Protected methods

void SpriteManager::DoUpdate()
{
	UpdateAnimationTimes();
	UpdateAnimatedTiles();
	UpdateScales();
	ApplyBlink();
	SimulateTintVelocity();
}


// Private methods

void SpriteManager::UpdateAnimationTimes()
{
	auto view = myRegistry.view<AnimationTime, const AnimationSpeed>();
	for (auto [entity, time, speed] : view.each())
		time.value += speed.value * myDeltaTime;
}

void SpriteManager::UpdateAnimatedTiles()
{
	auto view = myRegistry.view<Tile, Tileset* const, const AnimationTag, const AnimationTime>();
	for (auto [entity, frame, tileset, tag, time] : view.each())
	{
		if (!tileset || !tileset->HasTag(tag.value))
			continue;

		const unsigned milliseconds = static_cast<unsigned>(1000.f * time.value);
		frame.value = tileset->GetFrame(tag.value, milliseconds);
	}
}

void SpriteManager::UpdateScales()
{
	auto view = myRegistry.view<const Velocity, Scale>();
	for (auto [entity, velocity, scale] : view.each())
	{
		if (auto attack = myRegistry.try_get<AttackState>(entity))
		{
			if (*attack != AttackState::None)
				continue;
		}

		if (auto face = myRegistry.try_get<FaceGroundMotion>(entity))
		{
			if (!face->value)
				continue;
			else if (velocity.value.x > 0.f)
				scale.value.x = 1.f;
			else if (velocity.value.x < 0.f)
				scale.value.x = -1.f;
		}
	}
}

void SpriteManager::ApplyBlink()
{
	constexpr float blinkPeriod = 0.1f;

	// Blink for duration
	{
		auto view = myRegistry.view<Alpha, BlinkForDuration>();
		for (auto [entity, alpha, blinkDuration] : view.each())
		{
			blinkDuration.value -= myDeltaTime;
			if (blinkDuration.value > 0.f)
				alpha.value = std::roundf(std::fmodf(blinkDuration.value, blinkPeriod) / blinkPeriod);
			else
			{
				alpha.value = 1.f;
				myRegistry.remove<BlinkForDuration>(entity);
			}
		}
	}

	// Blink when lifespan less than
	{
		auto view = myRegistry.view<Alpha, const Lifespan, const BlinkWhenLifespanLessThan>();
		for (auto [entity, alpha, lifespan, lifespanUpperLimit] : view.each())
		{
			if (lifespan.value < lifespanUpperLimit.value)
				alpha.value = std::roundf(std::fmodf(lifespan.value, blinkPeriod) / blinkPeriod);
		}
	}
}

void SpriteManager::SimulateTintVelocity()
{
	auto view = myRegistry.view<Tint, const TintVelocity>();
	for (auto [entity, tint, velocity] : view.each())
	{
		tint.value += velocity.value * myDeltaTime;
		tint.value = saturate(tint.value);
	}
}
