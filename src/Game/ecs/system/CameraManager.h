#pragma once
#include "ecs/system/System.h"


class CameraManager : public System
{
public:
	CameraManager(Game&);

	void Update();

private:
	void FocusOnPlayer();
	void FitInLevelBounds();
	void SetScreenPositions();
};