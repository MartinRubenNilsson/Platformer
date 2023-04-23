#pragma once
#include "ecs/system/System.h"


class SpriteManager : public System
{
public:
	SpriteManager(class Game&);

protected:
	void DoUpdate() override;

private:
	void UpdateAnimationTimes();
	void UpdateAnimatedTiles();
	void UpdateScales();
	void ApplyBlink();
	void SimulateTintVelocity();

	const float myDeltaTime;
};