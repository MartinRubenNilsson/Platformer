#pragma once
#include "math/vec2.hpp"
#include "math/Rect.h"
#include <entt/entt.hpp>


entt::entity GetEntityWithIID(entt::registry&, size_t anIID);

Rect GetEntityBounds(entt::handle);
void ReplacePivotWithoutChangingBounds(entt::handle, float2 aPivot);
void AddChild(entt::handle aParent, entt::entity aChild, float2 aLocalPosition = { 0.f, 0.f });

void CopyOverwritePlayers(entt::registry& aDstReg, entt::registry& aSrcReg);

void CreateGameOverText(entt::registry&);

void DropCoinsUntilOutOfMoney(entt::handle);

void AddBlinkForDuration(entt::handle, float aDuration);
void AddHealthFlash(entt::handle);