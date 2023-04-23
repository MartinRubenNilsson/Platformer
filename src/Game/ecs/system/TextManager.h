#pragma once
#include "ecs/system/System.h"


class TextManager : public System
{
public:
	TextManager(Game&);

	void Update();

private:
	void CreateChars();
	void AddDepthToChars();
	void AddShakeToChars();
};