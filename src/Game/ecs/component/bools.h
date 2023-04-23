#pragma once

struct BoolComponent
{
	bool value = false;
};

struct Interacting : BoolComponent {};

struct FaceGroundMotion : BoolComponent {};
struct TurnOnTouchWall : BoolComponent {};
struct TurnOnTouchLedge : BoolComponent {};
struct MoveAlongSurface : BoolComponent {};