#pragma once
#include <algorithm> // clamp

struct Color
{
	float r = 0.f;
	float g = 0.f;
	float b = 0.f;
	float a = 0.f;

	Color& operator+=(const Color& color)
	{
		r += color.r;
		g += color.g;
		b += color.b;
		a += color.a;
		return *this;
	}

	Color& operator-=(const Color& color)
	{
		r -= color.r;
		g -= color.g;
		b -= color.b;
		a -= color.a;
		return *this;
	}
};

inline Color operator*(const float& f, const Color& color)
{
	return { f * color.r, f * color.g, f * color.b, f * color.a };
}

inline Color operator*(const Color& color, const float& f)
{
	return f * color;
}

inline Color saturate(const Color& color)
{
	return { std::clamp(color.r, 0.f, 1.f), std::clamp(color.g, 0.f, 1.f), std::clamp(color.b, 0.f, 1.f), std::clamp(color.a, 0.f, 1.f) };
}