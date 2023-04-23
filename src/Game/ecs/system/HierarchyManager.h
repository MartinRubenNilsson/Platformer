#pragma once
#include "ecs/system/System.h"
#include <entt/entt.hpp>
#include <unordered_map>


class HierarchyManager : public System
{
public:
	HierarchyManager(Game&);
	~HierarchyManager();

	void operator()(entt::entity); // For visiting
	bool operator()(entt::entity, entt::entity); // For comparing

	void Update();

private:
	static void OnDestroyChildren(entt::registry&, entt::entity);

	void SortHierarchy();
	void UpdatePositions();

	std::unordered_map<entt::entity, size_t> myVisitedEntities;
};