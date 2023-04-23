#include "pch.h"
#include "ecs/system/AudioManager.h"
#include "ecs/tags.h"
#include "ecs/component/ints.h"
#include "ecs/component/float2s.h"
#include "ecs/component/enums.h"
#include "ecs/component/Events.h"
#include "ecs/component/Sensors.h"

#include "audio/Listener.h"
#include "audio/Event.h"
#include "interface/FMODInterface.h"
#include <fmod/fmod_studio_guids.hpp>

using namespace FSPRO::Event;


// Constructor & destructor

AudioManager::AudioManager(Game& aGame)
	: System(aGame)
{
	myRegistry.on_construct<ConstructEvent>().connect<&AudioManager::OnConstructEvent>();
	myRegistry.on_construct<Tag::Dead>().connect<&AudioManager::OnConstructTagDead>();
	myRegistry.on_construct<Tag::Dealt>().connect<&AudioManager::OnConstructTagDealt>();
	myRegistry.on_destroy<DestroyEvent>().connect<&AudioManager::OnDestroyEvent>();
}

AudioManager::~AudioManager()
{
	myRegistry.on_construct<ConstructEvent>().disconnect<&AudioManager::OnConstructEvent>();
	myRegistry.on_construct<Tag::Dead>().disconnect<&AudioManager::OnConstructTagDead>();
	myRegistry.on_construct<Tag::Dealt>().disconnect<&AudioManager::OnConstructTagDealt>();
	myRegistry.on_destroy<DestroyEvent>().disconnect<&AudioManager::OnDestroyEvent>();
}


// Protected methods

void AudioManager::DoUpdate()
{
	UpdateListener();
	UpdatePlayerFootsteps();
}


// Private static methods

void AudioManager::OnConstructEvent(entt::registry& aRegistry, entt::entity anEntity)
{
	aRegistry.get<ConstructEvent>(anEntity).Start();
}

void AudioManager::OnConstructTagDead(entt::registry& aRegistry, entt::entity anEntity)
{
	if (DeathEvent* event = aRegistry.try_get<DeathEvent>(anEntity))
		event->Start();

	if (aRegistry.all_of<Tag::Player>(anEntity))
	{
		FMODInterface::StopAllEvents();
		FMODInterface::PlayEvent(&Game_Over);
	}
}

void AudioManager::OnConstructTagDealt(entt::registry& aReigstry, entt::entity anEntity)
{
	FMODInterface::PlayEvent(&Player_Sword_Hit);
}

void AudioManager::OnDestroyEvent(entt::registry& aRegistry, entt::entity anEntity)
{
	aRegistry.get<DestroyEvent>(anEntity).Start();
}


// Private non-static methods

void AudioManager::UpdateListener()
{
	for (auto [entity, listener, position] : myRegistry.view<Listener, const Position>().each())
		listener.position = position.value;

	for (auto [entity, listener, velocity] : myRegistry.view<Listener, const Velocity>().each())
		listener.velocity = velocity.value;

	for (auto [entity, listener] : myRegistry.view<const Listener>().each())
		listener.Set();
}

void AudioManager::UpdatePlayerFootsteps()
{
	auto view = myRegistry.view<
		Tag::Player,
		const Sensors,
		const Velocity,
		Event
	>(entt::exclude<Tag::Dead>);

	for (auto [player, sensors, velocity, event] : view.each())
	{
		if (velocity.value.x == 0.f)
			continue;

		if (event.GetPlaybackState() == PlaybackState::Playing &&
			event.GetTimelinePosition() < 150)
			continue;

		for (const Sensor& sensor : sensors)
		{
			if (sensor.dir == Direction::Down && sensor.dist == 0.f)
			{
				event.SetParameter("Material", (float)sensor.value);
				event.Start();
			}
		}
	}
}
