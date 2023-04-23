#pragma once
#include "ecs/system/System.h"
#include <entt/entt.hpp>


class AudioManager : public System
{
public:
	AudioManager(Game&);
	~AudioManager();

protected:
	void DoUpdate() override;

private:
	static void OnConstructEvent(entt::registry&, entt::entity);
	static void OnConstructTagDead(entt::registry&, entt::entity);
	static void OnConstructTagDealt(entt::registry&, entt::entity);
	static void OnDestroyEvent(entt::registry&, entt::entity);

	void UpdateListener();
	void UpdatePlayerFootsteps();
};