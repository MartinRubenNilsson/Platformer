#pragma once
#include <string>

struct StringComponent
{
	std::string value = {};
};

struct ColliderSlice : StringComponent {};
struct AnimationTag : StringComponent {};
struct Text : StringComponent {};
struct DestroyDrops : StringComponent {};