#pragma once
#include "math/vec2.hpp"

enum class Direction
{
	Right = 0,
	Down  = 1,
	Left  = 2,
	Up    = 3
};

inline bool Parallel(Direction u, Direction v)
{
	return (static_cast<unsigned>(u) + static_cast<unsigned>(v)) % 2u == 0u;
}

inline bool Perpendicular(Direction u, Direction v)
{
	return (static_cast<unsigned>(u) + static_cast<unsigned>(v)) % 2u == 1u;
}

inline Direction GetRotatedDirection(Direction dir, unsigned aNumberOfQuarterTurnsClockwise)
{
	return static_cast<Direction>((static_cast<unsigned>(dir) + aNumberOfQuarterTurnsClockwise) % 4u);
}

inline Direction GetOppositeDirection(Direction dir)
{
	return GetRotatedDirection(dir, 2);
}

inline int GetOrientation(Direction xAxis, Direction yAxis)
{
	return (static_cast<int>(yAxis) - static_cast<int>(xAxis)) % 2;
}

inline float2 GetUnitVectorInDirection(Direction dir)
{
	return { static_cast<float>((1 - static_cast<int>(dir)) % 2), static_cast<float>((2 - static_cast<int>(dir)) % 2) };
}

inline float2 GetNormalVectorToDirection(Direction dir)
{
	return GetUnitVectorInDirection(GetRotatedDirection(dir, 1));
}