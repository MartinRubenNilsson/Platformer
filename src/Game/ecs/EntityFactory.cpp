#include "pch.h"
#include "ecs/EntityFactory.h"
#include "ecs/tags.h"
#include "ecs/component/all_components.h"
#include "ldtk/LDtk.h"
#include "graphics/texture/Tileset.h"
#include "audio/Listener.h"
#include "math/Random.h"


using namespace entt::literals;


void from_json(const json& j, EntityIID& aComponent)
{
	aComponent.iid = std::hash<std::string>{}(j.at("iid").get<std::string>());
}

void from_json(const json& j, EntityRef& aComponent)
{
	aComponent.entityIid = std::hash<std::string>{}(j.at("entityIid").get<std::string>());
	aComponent.levelIid = std::hash<std::string>{}(j.at("levelIid").get<std::string>());
}

void from_json(const json& j, float2& aVector)
{
	const json k = j.is_object() ? j : j.is_string() ? json::parse(j.get<std::string>()) : json();
	if (k.contains("x"))
		k.at("x").get_to(aVector.x);
	if (k.contains("y"))
		k.at("y").get_to(aVector.y);
}

void from_json(const json& j, Direction& aDirection)
{
	std::string value = j.get<std::string>();
	if (value == "Right")
		aDirection = Direction::Right;
	else if (value == "Down")
		aDirection = Direction::Down;
	else if (value == "Left")
		aDirection = Direction::Left;
	else if (value == "Up")
		aDirection = Direction::Up;
	else
		assert(false);
}

void from_json(const json& j, Event& anEvent)
{
	anEvent.Create(j.get<std::string>());
}


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



// Constructors

EntityFactory::EntityFactory(entt::registry& aRegistry)
	: myRegistry(aRegistry)
	, myHandle{}
{
	myRegistry.on_construct<Speed>().connect<&entt::registry::emplace_or_replace<Velocity>>();
	myRegistry.on_construct<Gravity>().connect<&entt::registry::emplace_or_replace<Velocity>>();
	myRegistry.on_construct<Bounce>().connect<&entt::registry::emplace_or_replace<Velocity>>();
	myRegistry.on_construct<FaceGroundMotion>().connect<&entt::registry::emplace_or_replace<Scale>>();
	myRegistry.on_construct<BlinkWhenLifespanLessThan>().connect<&entt::registry::emplace_or_replace<Alpha>>();
	myRegistry.on_construct<ColliderScale>().connect<&entt::registry::emplace_or_replace<Collider>>();
	myRegistry.on_construct<SensorsScale>().connect<&entt::registry::emplace_or_replace<Sensors>>();
	myRegistry.on_construct<AnimationTag>().connect<&entt::registry::emplace_or_replace<AnimationTime>>();
	myRegistry.on_construct<AnimationTag>().connect<&entt::registry::emplace_or_replace<AnimationSpeed>>();
}

EntityFactory::~EntityFactory()
{
	myRegistry.on_construct<Speed>().disconnect<&entt::registry::emplace_or_replace<Velocity>>();
	myRegistry.on_construct<Gravity>().disconnect<&entt::registry::emplace_or_replace<Velocity>>();
	myRegistry.on_construct<Bounce>().disconnect<&entt::registry::emplace_or_replace<Velocity>>();
	myRegistry.on_construct<FaceGroundMotion>().disconnect<&entt::registry::emplace_or_replace<Scale>>();
	myRegistry.on_construct<BlinkWhenLifespanLessThan>().disconnect<&entt::registry::emplace_or_replace<Alpha>>();
	myRegistry.on_construct<ColliderScale>().disconnect<&entt::registry::emplace_or_replace<Collider>>();
	myRegistry.on_construct<SensorsScale>().disconnect<&entt::registry::emplace_or_replace<Sensors>>();
	myRegistry.on_construct<AnimationTag>().disconnect<&entt::registry::emplace_or_replace<AnimationTime>>();
	myRegistry.on_construct<AnimationTag>().disconnect<&entt::registry::emplace_or_replace<AnimationSpeed>>();
}


// Creation methods

entt::handle EntityFactory::CreateDefinition(const std::string& id, bool includeFields)
{
	myHandle = { myRegistry, myRegistry.create() };

	const EntityDef& def = LDtk::GetEntityDef(id);

	myHandle.emplace<EntityID>(&def.id);

	for (const std::string& tag : def.tags)
		Tag::Emplace(myHandle, tag.c_str());

	myHandle.emplace<Pivot>(def.pivot);
	myHandle.emplace<Size>(def.size);
	myHandle.emplace<Position>();

	if (def.tileset)
	{
		myHandle.emplace<Tileset*>(def.tileset);
		myHandle.emplace<Tile>(def.tile);
		myHandle.emplace<Depth>();
	}

	if (includeFields)
	{
		for (const auto& [id, value] : def.defaultFieldDefs.items())
			AddComponent(id, value);
	}

	AddSpecialComponents(id);

	return myHandle;
}

entt::handle EntityFactory::CreateInstance(const json& someData)
{
	myHandle = CreateDefinition(someData["id"].get<std::string>(), false);

	for (const auto& [id, value] : someData["customFields"].items())
		AddComponent(id, value);
	
	myHandle.emplace<EntityIID>(someData.get<EntityIID>());
	myHandle.emplace_or_replace<Depth>(LDtk::GetLayerDepth(someData["layer"].get<std::string>()));
	myHandle.emplace_or_replace<Position>(someData.get<float2>());

	return myHandle;
}

// Private methods

#define CASE(id) case #id ## _hs: myHandle.emplace<id>(value); break;

void EntityFactory::AddComponent(const std::string& id, const json& value)
{
	switch (entt::hashed_string(id.c_str()))
	{
		// Ints
	case "Health"_hs:
		myHandle.emplace<Health>(value);
		myHandle.emplace<PreviousHealth>(value);
		break;
	CASE(Score)
	CASE(Damage)
	CASE(IntGridValue)

		// Floats
	case "PositionX"_hs:
		myHandle.get<Position>().value.x = value;
		break;
	case "PositionY"_hs:
		myHandle.get<Position>().value.y = value;
		break;
	case "RandomFlipX"_hs:
		myHandle.get_or_emplace<Scale>().value.x = 1.f - 2.f * Random::Bernoulli(value);
		break;
	case "RandomFlipY"_hs:
		myHandle.get_or_emplace<Scale>().value.y = 1.f - 2.f * Random::Bernoulli(value);
		break;

	CASE(Speed)
	CASE(Gravity)
	CASE(Bounce)
	CASE(Lifespan)
	CASE(BlinkWhenLifespanLessThan)
	CASE(Shake)
	CASE(ColliderScale)
	CASE(SensorsScale)

	// Float2s
	CASE(ScreenPosition)

	// Booleans
	CASE(Interacting)
	CASE(FaceGroundMotion)
	CASE(TurnOnTouchWall)
	CASE(TurnOnTouchLedge)
	CASE(MoveAlongSurface)

	// Strings
	CASE(Text)
	CASE(AnimationTag)
	case "RandomAnimationTag"_hs:
	{
		auto tags = Split(value);
		if (auto tag = Random::Uniform(tags))
			myHandle.emplace<AnimationTag>(*tag);
		break;
	}
	case "RandomVelocity"_hs:
	{
		const json j = json::parse(value.get<std::string>());
		const float2 min = j.at("min").get<float2>();
		const float2 max = j.at("max").get<float2>();
		myHandle.emplace_or_replace<Velocity>(Random::UniformReal2(min, max));
		break;
	}
	CASE(DestroyDrops)

	// Events
	CASE(ConstructEvent)
	CASE(DeathEvent)
	CASE(DestroyEvent)

	// EntityRefs
	CASE(Exit)
	}

	if (id == "MoveDirection")
	{
		myHandle.emplace<MoveDirection>(value.get<Direction>());
		myHandle.emplace<FloorDirection>(Direction::Down);
		myHandle.emplace_or_replace<Rotation>();
		myHandle.emplace_or_replace<Scale>(1.f, 1.f);
	}
}

void EntityFactory::AddSpecialComponents(const std::string& id)
{
	if (id == "Player")
	{
		myHandle.emplace<PlayerTimers>();
		myHandle.emplace<PhysicsState>();
		myHandle.emplace<AttackState>();
		myHandle.emplace<Event>().Create("Player/Footstep");
	}
	else if (id == "Camera")
		myHandle.emplace<Listener>();
}
