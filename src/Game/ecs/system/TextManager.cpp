#include "pch.h"
#include "ecs/system/TextManager.h"
#include "ecs/tags.h"
#include "ecs/component/floats.h"
#include "ecs/component/float2s.h"
#include "ecs/component/strings.h"
#include "ecs/component/Rects.h"
#include "ecs/component/Children.h"
#include "Game.h"
#include "graphics/texture/Tileset.h"


// Constructor

TextManager::TextManager(Game& aGame)
	: System(aGame)
{
}


// Public methods

void TextManager::Update()
{
	CreateChars();
	AddDepthToChars();
	AddShakeToChars();
}


// Private methods

void TextManager::CreateChars()
{
	for (auto [entity] : myRegistry.view<Tag::Text>(entt::exclude<Children>).each())
		myRegistry.emplace<Children>(entity);

	auto view = myRegistry.view<
		Tag::Text,
		Children,
		const Text,
		Tileset* const,
		const Tile,
		const Position,
		const Pivot
	>();

	for (auto [entity, children, text, tileset, tile, position, pivot] : view.each())
	{
		if (!tileset)
		{
			myRegistry.destroy(children.begin(), children.end());
			children.clear();
			continue;
		}

		const float2 charSize = tileset->GetFrame(' ').GetSize();
		const size_t charCount = text.value.size() - rn::count(text.value, '\n');
		const size_t childCount = children.size();

		if (charCount > childCount)
		{
			children.resize(charCount);
			myRegistry.create(children.begin() + childCount, children.end());

			for (auto child = children.begin() + childCount; child != children.end(); ++child)
			{
				if (myRegistry.all_of<Tag::UI>(entity))
					myRegistry.emplace<Tag::UI>(*child);

				myRegistry.emplace<Pivot>(*child);
				myRegistry.emplace<Size>(*child, charSize);
				myRegistry.emplace<Position>(*child);
				myRegistry.emplace<LocalPosition>(*child);
				myRegistry.emplace<Tileset*>(*child, tileset);
				myRegistry.emplace<Tile>(*child);
			}
		}
		else if (charCount < childCount)
		{
			myRegistry.destroy(children.begin() + charCount, children.end());
			children.resize(charCount);
		}

		auto child = children.begin();
		float2 localPosition;

		std::istringstream ss(text.value);
		for (std::string line; std::getline(ss, line); localPosition.y += charSize.y)
		{
			localPosition.x = -pivot.value.x * charSize.x * line.size();

			for (char c : line)
			{
				myRegistry.replace<Tile>(*child, tileset->GetFrame(c));
				myRegistry.replace<LocalPosition>(*child, localPosition);

				++child;
				localPosition.x += charSize.x;
			}
		}
	}
}

void TextManager::AddDepthToChars()
{
	auto view = myRegistry.view<Tag::Text, const Children, const Depth>();
	for (auto [entity, children, depth] : view.each())
	{
		for (entt::entity child : children)
			myRegistry.emplace_or_replace<Depth>(child, depth);
	}
}

void TextManager::AddShakeToChars()
{
	// Remove shakes from characters
	{
		auto view = myRegistry.view<Tag::Text, const Children>(entt::exclude<Shake>);
		for (auto [entity, children] : view.each())
			myRegistry.remove<Shake>(children.begin(), children.end());
	}

	// Adds shakes to characters
	{
		auto view = myRegistry.view<Tag::Text, const Children, const Shake>();
		for (auto [entity, children, shake] : view.each())
		{
			for (entt::entity child : children)
				myRegistry.emplace_or_replace<Shake>(child, shake);
		}
	}
}
