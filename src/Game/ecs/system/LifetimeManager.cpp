#include "pch.h"
#include "ecs/system/LifetimeManager.h"
#include "ecs/EntityFactory.h"
#include "ecs/tags.h"
#include "ecs/component/floats.h"
#include "ecs/component/float2s.h"
#include "ecs/component/strings.h"
#include "Game.h"
#include "clock/Clock.h"
#include "level/Level.h"

namespace
{
	std::vector<std::string> Split(const std::string& aString)
	{
		std::istringstream ss(aString);
		std::vector<std::string> result;
		for (std::string s; ss >> s; result.emplace_back(s)) {}
		return result;
	}
}


// Constructor

LifetimeManager::LifetimeManager(Game& aGame)
	: System(aGame)
	, myDeltaTime(aGame.GetClock().GetDeltaTime())
{
	myRegistry.on_destroy<DestroyDrops>().connect<&LifetimeManager::OnDestroyDrops>();
}

LifetimeManager::~LifetimeManager()
{
	myRegistry.on_destroy<DestroyDrops>().disconnect<&LifetimeManager::OnDestroyDrops>();
}


// Protected methods

void LifetimeManager::DoUpdate()
{
	UpdateLifespans();
	DestroyParticlesOutOfBounds();
}


// Private static methods

void LifetimeManager::OnDestroyDrops(entt::registry& aRegistry, entt::entity anEntity)
{
	if (!aRegistry.all_of<Position>(anEntity))
		return;

	const Position position = aRegistry.get<Position>(anEntity);

	EntityFactory factory(aRegistry);
	for (const std::string& id : Split(aRegistry.get<DestroyDrops>(anEntity).value))
		factory.CreateDefinition(id).replace<Position>(position);
}


// Private methods

void LifetimeManager::UpdateLifespans()
{
	auto view = myRegistry.view<Lifespan>();
	for (auto [particle, lifespan] : view.each())
	{
		lifespan.value -= myDeltaTime;
		if (lifespan.value <= 0.f)
			myRegistry.destroy(particle);
	}
}

void LifetimeManager::DestroyParticlesOutOfBounds()
{
	const Rect bounds = myLevel.GetLocalBounds();

	auto view = myRegistry.view<Tag::Particle, const Size, const Pivot, const Position>();
	for (auto [particle, size, pivot, position] : view.each())
	{
		if (!Rect::Intersects(bounds, Rect::Create(size.value, pivot.value, position.value)))
			myRegistry.destroy(particle);
	}
}
