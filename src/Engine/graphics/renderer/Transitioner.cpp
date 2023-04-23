#include "pch.h"
#include "graphics/renderer/Transitioner.h"
#include "graphics/Graphics.h"
#include "graphics/cbuffer/ConstantBuffer.h"
#include "graphics/cbuffer/TransitionData.h"
#include "graphics/texture/Texture.h"
#include "graphics/texture/BackBuffer.h"
#include "graphics/state/StateManager.h"
#include "graphics/shader/ShaderManager.h"
#include "graphics/Rects.h"
#include "interface/DX11Interface.h"


// Constructor & destructor

Transitioner::Transitioner(Graphics& someGraphics)
	: myGraphics(someGraphics)
	, myTextureFrom(new Texture())
	, myTextureTo(new Texture())
	, myConstantBuffer(new ConstantBuffer())
	, myData()
	, myType(TransitionType::Push)
{}

Transitioner::~Transitioner() = default;


// Public methods

bool Transitioner::Init()
{
	if (!myConstantBuffer->Create(sizeof(TransitionData)))
		return false;

	return true;
}

bool Transitioner::SetSlideFrom(const Texture& aTexture, const Rect& aRegion)
{
	return myTextureFrom->Copy(aTexture, aRegion);
}

bool Transitioner::SetSlideTo(const Texture& aTexture, const Rect& aRegion)
{
	return myTextureTo->Copy(aTexture, aRegion);
}

void Transitioner::BeginPush(Direction aDirection)
{
	myType = TransitionType::Push;
	myData.direction = GetUnitVectorInDirection(aDirection);
}

void Transitioner::Render(float aProgress)
{
	if (myType == TransitionType::None)
		return;

	myTextureFrom->SetForPS(TEXTURE_SLOT_TRANSITION_OLD);
	myTextureTo->SetForPS(TEXTURE_SLOT_TRANSITION_NEW);

	myData.progress = aProgress;
	myConstantBuffer->Write(&myData);
	myConstantBuffer->SetForPS(CBUFFER_SLOT_TRANSITION);

	myGraphics.GetStateMgr().Set(BlendState::Disabled);

	myGraphics.GetShaderMgr().Set(ShaderType::FullscreenVS);
	myGraphics.GetShaderMgr().Set(ShaderType::TransitionPushPS);

	DX11Interface::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX11Interface::context->Draw(3, 0);
}

void Transitioner::End()
{
	myType = TransitionType::None;
}
