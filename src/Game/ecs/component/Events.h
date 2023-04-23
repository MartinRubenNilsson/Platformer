#pragma once
#include "audio/Event.h"


struct ConstructEvent : Event {};
struct DeathEvent : Event {};
struct DestroyEvent : Event {};