#pragma once
#include "math/vec2.hpp"
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <span>

using json = nlohmann::json;


class EntityFactory
{
public:
	EntityFactory(entt::registry&);
	~EntityFactory();

	entt::handle CreateDefinition(const std::string& id, bool includeFields = true);
	entt::handle CreateInstance(const json& someData);

private:
	void AddComponent(const std::string& id, const json& value);
	void AddSpecialComponents(const std::string& id);

	entt::registry& myRegistry;
	entt::handle myHandle;
};