#pragma once
#include "graphics/cbuffer/TransitionData.h"
#include "math/Rect.h"
#include "math/Direction.h"
#include <memory>

class Graphics;
class Texture;
class ConstantBuffer;


class Transitioner
{
public:
	Transitioner(Graphics&);
	~Transitioner();

	bool Init();

	bool SetSlideFrom(const Texture& aTexture, const Rect& aRegion);
	bool SetSlideTo(const Texture& aTexture, const Rect& aRegion);

	void BeginPush(Direction aDirection);
	void Render(float aProgress);
	void End();

private:
	enum class TransitionType
	{ 
		None,
		Push
	};

	Graphics& myGraphics;
	std::unique_ptr<Texture> myTextureFrom;
	std::unique_ptr<Texture> myTextureTo;
	std::unique_ptr<ConstantBuffer> myConstantBuffer;
	TransitionData myData;
	TransitionType myType;
};