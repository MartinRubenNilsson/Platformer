#pragma once
#include "math/vec2.hpp"
#include <span>


struct Rect
{
	float2 min = {};
	float2 max = {};

	bool Contains(float2 aPosition) const;
	float2 Sample(float2 uv) const;

	float GetWidth() const { return max.x - min.x; }
	float GetHeight() const { return max.y - min.y; }
	float2 GetSize() const { return max - min; }
	float2 GetCenter() const { return (max + min) / 2.f; }

	Rect GetTranslated(float2 anOffset) const;
	Rect GetRotated(float anAngle) const; // Will in general create a rect of greater size
	Rect GetSubrect(Rect uv) const;

	void Translate(float2 anOffset);
	void Rotate(float anAngle); // Will in general increase size
	void Scale(float2 aScale);

	static Rect Create(float2 aSize, float2 aPivot, float2 aPosition = { 0.f, 0.f });
	static Rect CreateBoundingRect(const std::span<float2> somePoints);
	static bool Intersects(const Rect& a, const Rect& b);
	static Rect GetIntersection(const Rect& a, const Rect& b);
};
