#include "pch.h"
#include "HierarchyManager.h"
#include "ecs/component/Children.h"
#include "ecs/component/float2s.h"
#include "Game.h"


// Constructor

HierarchyManager::HierarchyManager(Game& aGame)
	: System(aGame)
{
	myRegistry.on_destroy<Children>().connect<&HierarchyManager::OnDestroyChildren>();
}

HierarchyManager::~HierarchyManager()
{
	myRegistry.on_destroy<Children>().disconnect<&HierarchyManager::OnDestroyChildren>();
}


// Operators

void HierarchyManager::operator()(entt::entity anEntity)
{
	if (myVisitedEntities.contains(anEntity))
		return;

	if (auto children = myRegistry.try_get<Children>(anEntity))
	{
		for (size_t i = 0; i < children->size();)
		{
			entt::entity& child = (*children)[i];

			if (myRegistry.valid(child))
			{
				operator()(child);
				++i;
			}
			else
			{
				std::swap(child, children->back());
				children->pop_back();
			}
		}
	}

	myVisitedEntities.emplace(anEntity, myVisitedEntities.size());
}

bool HierarchyManager::operator()(entt::entity lhs, entt::entity rhs)
{
	return myVisitedEntities.at(lhs) >= myVisitedEntities.at(rhs);
}


// Public methods

void HierarchyManager::Update()
{
	SortHierarchy();
	UpdatePositions();
}


// Private methods

void HierarchyManager::OnDestroyChildren(entt::registry& aRegistry, entt::entity anEntity)
{
	Children& children = aRegistry.get<Children>(anEntity);
	aRegistry.destroy(children.begin(), children.end());
}

void HierarchyManager::SortHierarchy()
{
	myVisitedEntities.clear();
	myRegistry.view<Children>().each(std::ref(*this));
	myRegistry.sort<Children>(std::ref(*this));
}

void HierarchyManager::UpdatePositions()
{
	auto view = myRegistry.view<const Children, const Position>().use<const Children>();
	for (auto [entity, children, position] : view.each())
	{
		for (entt::entity child : children)
		{
			if (!myRegistry.all_of<Position>(child))
				continue;

			Position& childPosition = myRegistry.get<Position>(child);
			childPosition.value = position.value;

			if (auto localPosition = myRegistry.try_get<LocalPosition>(child))
				childPosition.value += localPosition->value;
		}
	}
}
