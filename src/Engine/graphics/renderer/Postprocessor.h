#pragma once
#include <memory>

class Graphics;
class RenderTarget;
class ConstantBuffer;
struct PostprocessData;


class Postprocessor
{
public:
	Postprocessor(Graphics&);

	bool Init();
	void Render(const PostprocessData&);

private:
	Graphics& myGraphics;
	std::unique_ptr<RenderTarget> myRenderTarget;
	std::unique_ptr<ConstantBuffer> myPostprocessBuffer;
};