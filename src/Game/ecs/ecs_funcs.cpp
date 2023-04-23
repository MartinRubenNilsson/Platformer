#include "pch.h"
#include "ecs/ecs_funcs.h"
#include "ecs/tags.h"
#include "ecs/component/all_components.h"
#include "ecs/EntityFactory.h"
#include "ecs/EntityCopier.h"


entt::entity GetEntityWithIID(entt::registry& aRegistry, size_t anIID)
{
	auto view = aRegistry.view<const EntityIID>();
	for (auto [entity, iid] : view.each())
	{
		if (iid.iid == anIID)
			return entity;
	}
	return entt::null;
}

Rect GetEntityBounds(entt::handle aHandle)
{
	return Rect::Create(aHandle.get<Size>().value, aHandle.get<Pivot>().value, aHandle.get<Position>().value);
}

void ReplacePivotWithoutChangingBounds(entt::handle aHandle, float2 aPivot)
{
	aHandle.replace<Position>(GetEntityBounds(aHandle).Sample(aPivot));
	aHandle.replace<Pivot>(aPivot);
}

void AddChild(entt::handle aParent, entt::entity aChild, float2 aLocalPosition)
{
	if (!aParent.all_of<Children>())
		aParent.emplace<Children>();
	Children& children = aParent.get<Children>();
	assert(rn::count(children, aChild) == 0);
	children.push_back(aChild);
	aParent.registry()->emplace<LocalPosition>(aChild, aLocalPosition);
}



void CopyOverwritePlayers(entt::registry& aDstReg, entt::registry& aSrcReg)
{
	std::vector<entt::entity> existingPlayers;
	for (auto [player] : aDstReg.view<Tag::Player>().each())
		existingPlayers.push_back(player);

	EntityCopier copier(aDstReg, aSrcReg);
	for (auto [player] : aSrcReg.view<Tag::Player>().each())
		copier.ShallowCopy(player);

	aDstReg.destroy(existingPlayers.begin(), existingPlayers.end());
}

void CreateGameOverText(entt::registry& aRegistry)
{
	EntityFactory builder(aRegistry);
	entt::handle text = builder.CreateDefinition("GameOverText");

	for (auto [entity, position] : aRegistry.view<Tag::Camera, Position>().each())
		text.replace<Position>(position);
}



void DropCoinsUntilOutOfMoney(entt::handle aHandle)
{
	static const std::pair<std::string, int> coinDefs[] =
	{
		{ "GoldCoin",   7  },
		{ "SilverCoin", 5  },
		{ "RedCoin",    3  },
		{ "BlueCoin",   1  }
	};

	if (!aHandle.all_of<Score>() || aHandle.get<Score>().value <= 0)
		return;

	EntityFactory factory(*aHandle.registry());

	for (int value = aHandle.get<Score>().value; value > 0;)
	{
		bool dropped = false;

		for (const auto& coinDef : coinDefs)
		{
			if (coinDef.second > value)
				continue;
			
			dropped = true;
			value -= coinDef.second;

			factory.CreateDefinition(coinDef.first)
				.replace<Position>(aHandle.get<Position>());

			break;
		}

		assert(dropped);
	}

	aHandle.replace<Score>(0);
}

void AddBlinkForDuration(entt::handle aHandle, float aDuration)
{
	aHandle.emplace_or_replace<Alpha>(1.f);
	aHandle.emplace_or_replace<BlinkForDuration>(aDuration);
}

void AddHealthFlash(entt::handle aHandle)
{
	aHandle.emplace_or_replace<Tint>(1.f, 0.5f, 0.5f, 0.f); // rgba
	aHandle.emplace_or_replace<TintVelocity>(-5.f, -5.f, -5.f, 0.f);
}
