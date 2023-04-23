#include "pch.h"
#include "ecs/system/CollisionManager.h"
#include "ecs/EntityFactory.h"
#include "ecs/tags.h"
#include "ecs/component/ints.h"
#include "ecs/component/floats.h"
#include "ecs/component/float2s.h"
#include "ecs/component/strings.h"
#include "ecs/component/Rects.h"
#include "ecs/component/c_player.h"
#include "Game.h"
#include "level/Level.h"
#include "grid/IntGrid.h"


namespace
{
	void SendCorpseFlying(entt::handle aCorpse, entt::handle aWeapon)
	{
		const Rect corpseCollider = aCorpse.get<Collider>().value;
		const Rect weaponCollider = aWeapon.get<Collider>().value;
		const Rect woundCollider = Rect::GetIntersection(corpseCollider, weaponCollider);

		const float2 corpseToWound = normalize(woundCollider.GetCenter() - corpseCollider.GetCenter());
		const float2 weaponToWound = normalize(woundCollider.GetCenter() - weaponCollider.GetCenter());

		const float2 velocity = 200.f * weaponToWound;
		const float angularVelocity = 25.f * determinant(corpseToWound, weaponToWound);

		aCorpse.emplace_or_replace<Velocity>(velocity);
		aCorpse.emplace_or_replace<AngularVelocity>(angularVelocity);

		if (!aCorpse.all_of<Rotation>())
			aCorpse.emplace<Rotation>();
	}
}



// Constructor

CollisionManager::CollisionManager(Game& aGame)
	: System(aGame)
	, myGrid(myLevel.GetIntGrid())
{}


// Protected methods

void CollisionManager::DoUpdate()
{
	PrepareColliders();

	PlayerVsHealthPickups();
	PlayerVsScorePickups();
	PlayerVsEnemies();

	DamageVsEnemies();
	DamageVsDestructibles();
}


// Public methods

void CollisionManager::PrepareColliders()
{
	// Set size and pivot
	for (auto [entity, collider, scale, size, pivot] : myRegistry.view<Collider, const ColliderScale, const Size, const Pivot>().each())
		collider.value = Rect::Create(scale.value * size.value, pivot.value);

	// Scale
	for (auto [entity, collider, scale] : myRegistry.view<Collider, const Scale>().each())
		collider.value.Scale(scale.value);

	// Rotate
	for (auto [entity, collider, rotation] : myRegistry.view<Collider, const Rotation>().each())
		collider.value.Rotate(rotation.value);

	// Translate
	for (auto [entity, collider, position] : myRegistry.view<Collider, const Position>().each())
		collider.value.Translate(position.value);
}

void CollisionManager::PlayerVsHealthPickups()
{
	for (auto [player, playerCollider, playerHealth]
		: myRegistry.view<Tag::Player, const Collider, const Health>(entt::exclude<Tag::Dead>).each())
	{
		for (auto [pickup, pickupCollider, pickupHealth]
			: myRegistry.view<Tag::Pickup, const Collider, const Health>().each())
		{
			if (!Rect::Intersects(playerCollider.value, pickupCollider.value))
				continue;

			myRegistry.replace<Health>(player, playerHealth.value + pickupHealth.value);
			myRegistry.destroy(pickup);
			return;
		}
	}
}

void CollisionManager::PlayerVsScorePickups()
{
	for (auto [player, playerCollider, playerScore]
		: myRegistry.view<Tag::Player, const Collider, const Score>(entt::exclude<Tag::Dead>).each())
	{
		for (auto [pickup, pickupCollider, pickupScore]
			: myRegistry.view<Tag::Pickup, const Collider, const Score>().each())
		{
			if (!Rect::Intersects(playerCollider.value, pickupCollider.value))
				continue;

			myRegistry.replace<Score>(player, playerScore.value + pickupScore.value);
			myRegistry.destroy(pickup);
			return;
		}
	}
}

void CollisionManager::PlayerVsEnemies()
{
	for (auto [player, timers] : myRegistry.view<Tag::Player, const PlayerTimers>().each())
	{
		if (timers.time[PlayerTimers::Invincibility] > 0.f)
			return;
	}

	for (auto [player, playerCollider, health]
		: myRegistry.view<Tag::Player, const Collider, const Health>(entt::exclude<Tag::Dead>).each())
	{
		for (auto [enemy, enemyCollider]
			: myRegistry.view<Tag::Enemy, const Collider>(entt::exclude<Tag::Dead>).each())
		{
			if (!Rect::Intersects(playerCollider.value, enemyCollider.value))
				continue;

			myRegistry.replace<Health>(player, health.value - 1);
			if (health.value <= 0)
			{
				SendCorpseFlying({ myRegistry, player }, { myRegistry, enemy });
				myRegistry.emplace<Tag::Dead>(player);
			}

			return;
		}
	}
}

void CollisionManager::DamageVsEnemies()
{
	for (auto [damageEntity, damageCollider, damage, damagePosition]
		: myRegistry.view<const Collider, const Damage, const Position>().each())
	{
		for (auto [enemyEntity, enemyCollider, health]
			: myRegistry.view<Tag::Enemy, const Collider, const Health>(entt::exclude<Tag::Dead>).each())
		{
			if (!Rect::Intersects(damageCollider.value, enemyCollider.value))
				continue;

			myRegistry.replace<Health>(enemyEntity, health.value - damage.value);
			if (health.value <= 0)
			{
				SendCorpseFlying({ myRegistry, enemyEntity }, { myRegistry, damageEntity });
				myRegistry.emplace<Tag::Dead>(enemyEntity);
			}

			myRegistry.remove<Collider>(damageEntity);
			myRegistry.remove<Damage>(damageEntity);
			myRegistry.emplace<Tag::Dealt>(damageEntity);
			return;
		}
	}
}

void CollisionManager::DamageVsDestructibles()
{
	for (auto [damageEntity, damage, damageCollider, damagePosition]
		: myRegistry.view<const Damage, const Collider, const Position>().each())
	{
		std::vector<entt::entity> destructibles;
		for (auto [destEntity, destCollider] : myRegistry.view<Tag::Destructible, const Collider>().each())
		{
			if (Rect::Intersects(damageCollider.value, destCollider.value))
				destructibles.push_back(destEntity);
		}

		if (destructibles.empty())
			continue;

		myRegistry.remove<Collider>(damageEntity);
		myRegistry.remove<Damage>(damageEntity);
		myRegistry.emplace<Tag::Dealt>(damageEntity);

		const auto distanceToDamage = [this, damagePosition](entt::entity entity)
		{
			return length(myRegistry.get<Position>(entity).value - damagePosition.value);
		};

		rn::sort(destructibles, {}, distanceToDamage);

		entt::entity closestDestructible = destructibles.front();
		const float2 position = myRegistry.get<Position>(closestDestructible).value;

		if (myRegistry.all_of<IntGridValue>(closestDestructible))
			myGrid.SetValue(position, 0);

		myRegistry.remove<DestroyDrops>(closestDestructible);
		myRegistry.destroy(closestDestructible);
		
		return;
	}
}
