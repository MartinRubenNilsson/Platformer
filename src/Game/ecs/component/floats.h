#pragma once

struct FloatComponent
{
	float value = 0.f;
};

struct Rotation : FloatComponent {};
struct AngularVelocity : FloatComponent {};
struct Bounce : FloatComponent {};
struct Gravity : FloatComponent {};
struct Depth : FloatComponent {};
struct ColliderScale : FloatComponent {};
struct SensorsScale : FloatComponent {};
struct Lifespan : FloatComponent {};
struct Speed : FloatComponent {};
struct Alpha { float value = 1.f; };
struct AnimationTime : FloatComponent {};
struct AnimationSpeed { float value = 1.f; };
struct BlinkForDuration : FloatComponent {};
struct BlinkWhenLifespanLessThan : FloatComponent {};
struct Shake : FloatComponent {};