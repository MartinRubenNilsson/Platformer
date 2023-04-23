#pragma once
#include "math/Direction.h"

struct DirectionComponent
{
	Direction value;
};

struct MoveDirection : DirectionComponent {};
struct FloorDirection : DirectionComponent {};