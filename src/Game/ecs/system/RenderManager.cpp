#include "pch.h"
#include "ecs/system/RenderManager.h"

#include "ecs/tags.h"
#include "ecs/component/floats.h"
#include "ecs/component/float2s.h"
#include "ecs/component/Colors.h"
#include "ecs/component/Rects.h"

#include "Game.h"
#include "level/LevelManager.h"

#include "Engine.h"
#include "clock/Clock.h"
#include "graphics/Graphics.h"
#include "graphics/Rects.h"
#include "graphics/texture/BackBuffer.h"
#include "graphics/texture/Tileset.h"
#include "graphics/cbuffer/ConstantBuffer.h"
#include "graphics/cbuffer/CameraData.h"
#include "graphics/cbuffer/SpriteData.h"
#include "graphics/cbuffer/LineStripData.h"
#include "graphics/cbuffer/PostprocessData.h"
#include "graphics/renderer/Drawer.h"
#include "graphics/renderer/Transitioner.h"
#include "graphics/renderer/Postprocessor.h"

#include "math/Random.h"
#include "math/math_funcs.h"


// Constructor

RenderManager::RenderManager(Game& aGame)
	: System(aGame)
	, myGraphics(aGame.GetEngine().GetGraphics())
	, myDrawer(myGraphics.GetDrawer())
	, myCameraData()
	, myPostprocessData{ Color(1.f, 1.f, 1.f, 1.f) }
{
}


// Public methods

void RenderManager::DoUpdate()
{
	NewFrame();
	SortByDepth();
	DrawSprites();
	Transition();
	Postprocess();
	DrawUI();
}


// Private methods

void RenderManager::NewFrame()
{
	auto view = myRegistry.view<Tag::Camera, const Size, const Position>();
	for (auto [entity, size, position] : view.each())
	{
		myCameraData.size = size.value;
		myCameraData.position = position.value;
		myGraphics.GetCameraBuffer().Write(&myCameraData);

		const uint2 backBufferSize = myGraphics.GetBackBuffer().GetSize();

		myViewport.max = GetRescaledToFitExactly(size.value, (float2)backBufferSize); // Add letter- or pillarboxing if necessary
		myViewport.Translate((backBufferSize - myViewport.max) / 2.f); // Move to center of backbuffer
		myViewport.Set();

		ScissorRect scissorRect(myViewport);
		scissorRect.Set();
	}
}

void RenderManager::SortByDepth()
{
	myRegistry.sort<Depth>([](const Depth& lhs, const Depth& rhs) { return lhs.value > rhs.value; });
}

void RenderManager::DrawSprites()
{
	auto view = myRegistry.view<
		Tileset* const,
		const Tile,
		const Pivot,
		const Size,
		const Position,
		const Depth
	>(entt::exclude<Tag::UI, Tag::Text>).use<const Depth>();

	for (auto [entity, tileset, tile, pivot, size, position, depth] : view.each())
	{
		if (!tileset)
			continue;

		SpriteData sprite = {};
		sprite.uv.min = tile.value.min / tileset->GetSize();
		sprite.uv.max = tile.value.max / tileset->GetSize();
		sprite.pivot = pivot.value;
		sprite.size = size.value;
		sprite.position = position.value;

		sprite.rotation = 0.f;
		sprite.alpha = 1.f;
		sprite.tint = { 0.f, 0.f, 0.f, 0.f };

		if (auto scale = myRegistry.try_get<const Scale>(entity))
			sprite.size *= scale->value;

		if (auto rotation = myRegistry.try_get<const Rotation>(entity))
			sprite.rotation = rotation->value;

		if (auto alpha = myRegistry.try_get<const Alpha>(entity))
			sprite.alpha = alpha->value;

		if (auto tint = myRegistry.try_get<const Tint>(entity))
			sprite.tint = tint->value;

		if (auto shake = myRegistry.try_get<const Shake>(entity))
			sprite.position += Random::Normal2({}, { shake->value, shake->value });

		myDrawer.DrawSprite(tileset, &sprite);
	}
}


void RenderManager::DrawColliders()
{
	auto view = myRegistry.view<const Collider>();
	for (auto [entity, collider] : view.each())
	{
		LineStripData lineStrip = {};
		lineStrip.vertices[0].xy = collider.value.min;
		lineStrip.vertices[1].xy = { collider.value.max.x, collider.value.min.y };
		lineStrip.vertices[2].xy = collider.value.max;
		lineStrip.vertices[3].xy = { collider.value.min.x, collider.value.max.y };
		lineStrip.vertices[4].xy = collider.value.min;

		myDrawer.DrawLineStrip(&lineStrip);
	}
}

void RenderManager::Transition()
{
	BackBuffer& backBuffer = myGraphics.GetBackBuffer();
	Transitioner& transitioner = myGraphics.GetTransitioner();

	Level* currLevel = myGame.GetLevelMgr().GetCurrentLevel();
	Level* nextLevel = myGame.GetLevelMgr().GetNextLevel();
	Direction direction;

	if (currLevel && nextLevel && currLevel->HasNeighbour(*nextLevel, &direction))
	{
		transitioner.SetSlideFrom(backBuffer, myViewport);
		transitioner.BeginPush(direction);
	}

	const float progress = myGame.GetClock().GetPauseProgress();

	if (progress < 1.f)
	{
		transitioner.SetSlideTo(backBuffer, myViewport);
		transitioner.Render(progress);
	}
	else
		transitioner.End();
}

void RenderManager::Postprocess()
{
	if (ImGui::Begin("Postprocess"))
	{
		ImGui::ColorPicker4("Tint", &myPostprocessData.tint.r);
		ImGui::End();
	}

	myGraphics.GetPostprocessor().Render(myPostprocessData);
}

void RenderManager::DrawUI()
{
	myViewport.Set();

	auto view = myRegistry.view<
		Tag::UI,
		Tileset* const,
		const Tile,
		const Pivot,
		const Size,
		const Position
	>(entt::exclude<Tag::Text>);

	for (auto [entity, tileset, tile, pivot, size, position] : view.each())
	{
		if (!tileset)
			continue;

		SpriteData sprite = {};
		sprite.uv.min = tile.value.min / tileset->GetSize();
		sprite.uv.max = tile.value.max / tileset->GetSize();
		sprite.pivot = pivot.value;
		sprite.size = size.value;
		sprite.position = position.value;

		sprite.rotation = 0.f;
		sprite.alpha = 1.f;
		sprite.tint = { 0.f, 0.f, 0.f, 0.f };

		if (auto scale = myRegistry.try_get<const Scale>(entity))
			sprite.size *= scale->value;

		if (auto rotation = myRegistry.try_get<const Rotation>(entity))
			sprite.rotation = rotation->value;

		myDrawer.DrawSprite(tileset, &sprite);
	}
}
