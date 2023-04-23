#include "pch.h"
#include "graphics/renderer/Postprocessor.h"

#include "graphics/Graphics.h"
#include "graphics/texture/BackBuffer.h"
#include "graphics/texture/RenderTarget.h"
#include "graphics/cbuffer/ConstantBuffer.h"
#include "graphics/cbuffer/PostprocessData.h"
#include "graphics/shader/ShaderManager.h"
#include "graphics/state/StateManager.h"
#include "graphics/Rects.h"
#include "interface/DX11Interface.h"



// Constructor & destructor

Postprocessor::Postprocessor(Graphics& someGraphics)
	: myGraphics(someGraphics)
	, myRenderTarget(new RenderTarget())
	, myPostprocessBuffer(new ConstantBuffer())
{}


// Public methods

bool Postprocessor::Init()
{
	if (!myRenderTarget->Create(myGraphics.GetBackBuffer().GetSize()))
		return false;

	if (!myPostprocessBuffer->Create(sizeof(PostprocessData)))
		return false;

	return true;
}

void Postprocessor::Render(const PostprocessData& someData)
{
	const uint2 size = myGraphics.GetBackBuffer().GetSize();
	if (myRenderTarget->GetSize() != size)
		myRenderTarget->Create(size);

	myPostprocessBuffer->SetForPS(CBUFFER_SLOT_POSTPROCESS);
	myPostprocessBuffer->Write(&someData);

	myRenderTarget->Copy(myGraphics.GetBackBuffer());
	myGraphics.GetBackBuffer().SetAsTarget();
	myRenderTarget->SetForPS(TEXTURE_SLOT_POSTPROCESS);
	
	myGraphics.GetShaderMgr().Set(ShaderType::FullscreenVS);
	myGraphics.GetShaderMgr().Set(ShaderType::PostprocessPS);
	myGraphics.GetStateMgr().Set(BlendState::Disabled);

	Viewport{ float2(), (float2)size }.Set();

	DX11Interface::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX11Interface::context->Draw(3, 0);
}
