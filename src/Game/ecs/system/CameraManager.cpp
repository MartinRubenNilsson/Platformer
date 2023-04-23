#include "pch.h"
#include "ecs/system/CameraManager.h"
#include "ecs/tags.h"
#include "ecs/component/float2s.h"
#include "level/Level.h"


// Constructor

CameraManager::CameraManager(Game& aGame)
	: System(aGame)
{}


// Public methods

void CameraManager::Update()
{
	FocusOnPlayer();
	FitInLevelBounds();
	SetScreenPositions();
}


// Private methods

void CameraManager::FocusOnPlayer()
{
	for (auto [camera, cameraPosition]
		: myRegistry.view<Tag::Camera, Position>().each())
	{
		for (auto [player, playerPosition]
			: myRegistry.view<Tag::Player, const Position>(entt::exclude<Tag::Dead>).each())
			cameraPosition.value = playerPosition.value;
	}
}

void CameraManager::FitInLevelBounds()
{
	const float2 levelSize = (float2)myLevel.GetSize();

	auto view = myRegistry.view<Tag::Camera, Size, Position>();
	for (auto [entity, size, position] : view.each())
	{
		size.value.x = std::min(size.value.x, levelSize.x);
		size.value.y = std::min(size.value.y, levelSize.y);

		const float2 cameraMin = position.value - size.value / 2.f;
		const float2 cameraMax = position.value + size.value / 2.f;

		if (cameraMin.x < 0.f)
			position.value.x -= cameraMin.x;
		else if (cameraMax.x > levelSize.x)
			position.value.x -= cameraMax.x - levelSize.x;

		if (cameraMin.y < 0.f)
			position.value.y -= cameraMin.y;
		else if (cameraMax.y > levelSize.y)
			position.value.y -= cameraMax.y - levelSize.y;
	}
}

void CameraManager::SetScreenPositions()
{
	for (auto [camera, cameraPosition, cameraSize]
		: myRegistry.view<Tag::Camera, const Position, const Size>().each())
	{
		for (auto [entity, position, screenPosition]
			: myRegistry.view<Position, const ScreenPosition>().each())
			position.value = cameraPosition.value + screenPosition.value * cameraSize.value / 2.f;
	}
}
