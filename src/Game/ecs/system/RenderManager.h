#pragma once
#include "ecs/system/System.h"
#include "graphics/cbuffer/CameraData.h"
#include "graphics/cbuffer/PostprocessData.h"
#include "graphics/Rects.h"


class RenderManager : public System
{
public:
	RenderManager(Game& aGame);

protected:
	void DoUpdate() override;

private:
	void NewFrame();
	void SortByDepth();
	void DrawSprites();
	void DrawColliders();
	void Transition();
	void Postprocess();
	void DrawUI();

	class Graphics& myGraphics;
	class Drawer& myDrawer;

	Viewport myViewport;
	CameraData myCameraData;
	PostprocessData myPostprocessData;
};