#pragma once

struct IntComponent
{
	int value = 0;
};

struct Health : IntComponent {};
struct PreviousHealth : IntComponent {};
struct Score : IntComponent {};
struct Damage : IntComponent {};
struct IntGridValue : IntComponent {};