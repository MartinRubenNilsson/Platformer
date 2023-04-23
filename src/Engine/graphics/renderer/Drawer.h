#pragma once
#include <memory>

class Graphics;
class Texture;
class ShaderManager;
class ConstantBuffer;

struct SpriteData;
struct LineStripData;


class Drawer
{
public:
	Drawer(Graphics&);

	bool Init();
	void DrawSprite(const Texture*, const SpriteData*);
	void DrawLineStrip(const LineStripData*);

private:
	Graphics& myGraphics;
	ShaderManager& myShaderMgr;
	std::unique_ptr<ConstantBuffer> mySpriteBuffer;
	std::unique_ptr<ConstantBuffer> myLineStripBuffer;
};