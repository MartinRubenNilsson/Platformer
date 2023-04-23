#include "pch.h"
#include "math/Rect.h"


// Queries

bool Rect::Contains(float2 aPosition) const
{
	return min.x <= aPosition.x && aPosition.x <= max.x && min.y <= aPosition.y && aPosition.y <= max.y;
}

float2 Rect::Sample(float2 uv) const
{
	return min * (1.f - uv) + max * uv;
}


// Getters

Rect Rect::GetTranslated(float2 anOffset) const
{
	return { min + anOffset, max + anOffset };
}

Rect Rect::GetRotated(float anAngle) const
{
	float2 rotatedCorners[4] =
	{
		rotate(min, anAngle), // top left
		rotate(max, anAngle), // bottom right
		rotate({ min.x, max.y }, anAngle), // bottom left
		rotate({ max.x, min.y }, anAngle) // top right
	};
	return CreateBoundingRect(rotatedCorners);
}

Rect Rect::GetSubrect(Rect uv) const
{
	Rect subrect = {};
	subrect.min.x = std::lerp(min.x, max.x, uv.min.x);
	subrect.min.y = std::lerp(min.y, max.y, uv.min.y);
	subrect.max.x = std::lerp(min.x, max.x, uv.max.x);
	subrect.max.y = std::lerp(min.y, max.y, uv.max.y);
	return subrect;
}


// Modifiers

void Rect::Translate(float2 anOffset)
{
	min += anOffset;
	max += anOffset;
}

void Rect::Rotate(float anAngle)
{
	*this = GetRotated(anAngle);
}

void Rect::Scale(float2 aScale)
{
	min *= aScale;
	max *= aScale;
	if (min.x > max.x) std::swap(min.x, max.x);
	if (min.y > max.y) std::swap(min.y, max.y);
}


// Static methods

Rect Rect::Create(float2 aSize, float2 aPivot, float2 aPosition)
{
	Rect result = { { 0.f, 0.f }, aSize };
	result.Translate(aPosition - aSize * aPivot);
	return result;
}

Rect Rect::CreateBoundingRect(const std::span<float2> somePoints)
{
	float2 min = { 0.f, 0.f };
	float2 max = { 0.f, 0.f };
	for (float2 p : somePoints)
	{
		if      (p.x < min.x) min.x = p.x;
		else if (p.x > max.x) max.x = p.x;
		if      (p.y < min.y) min.y = p.y;
		else if (p.y > max.y) max.y = p.y;
	}
	return { min, max };
}

bool Rect::Intersects(const Rect& a, const Rect& b)
{
	return !(
		a.max.x < b.min.x ||
		b.max.x < a.min.x ||
		a.max.y < b.min.y ||
		b.max.y < a.min.y
	);
}

Rect Rect::GetIntersection(const Rect& a, const Rect& b)
{
	return { ::max(a.min, b.min), ::min(a.max, b.max) };
}
