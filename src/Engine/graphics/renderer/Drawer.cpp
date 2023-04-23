#include "pch.h"
#include "graphics/renderer/Drawer.h"
#include "graphics/Graphics.h"
#include "graphics/state/StateManager.h"
#include "graphics/shader/ShaderManager.h"
#include "graphics/texture/Texture.h"
#include "graphics/cbuffer/ConstantBuffer.h"
#include "graphics/cbuffer/SpriteData.h"
#include "graphics/cbuffer/LineStripData.h"
#include "interface/DX11Interface.h"


// Constructor

Drawer::Drawer(Graphics& someGraphics)
	: myGraphics(someGraphics)
	, myShaderMgr(myGraphics.GetShaderMgr())
	, mySpriteBuffer(new ConstantBuffer())
	, myLineStripBuffer(new ConstantBuffer())
{}

// Public methods

bool Drawer::Init()
{
	if (!mySpriteBuffer->Create(sizeof(SpriteData)))
		return false;

	if (!myLineStripBuffer->Create(sizeof(LineStripData)))
		return false;

	return true;
}

void Drawer::DrawSprite(const Texture* aTexture, const SpriteData* someData)
{
	aTexture->SetForPS(TEXTURE_SLOT_SPRITE);
	mySpriteBuffer->Write(someData);
	mySpriteBuffer->SetForVS(CBUFFER_SLOT_SPRITE);
	mySpriteBuffer->SetForPS(CBUFFER_SLOT_SPRITE);
	myGraphics.GetStateMgr().Set(BlendState::Enabled);
	myShaderMgr.Set(ShaderType::SpriteVS);
	myShaderMgr.Set(ShaderType::SpritePS);
	DX11Interface::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX11Interface::context->Draw(6, 0);
}

void Drawer::DrawLineStrip(const LineStripData* someData)
{
	myLineStripBuffer->Write(someData);
	myLineStripBuffer->SetForVS(CBUFFER_SLOT_LINESTRIP);
	myShaderMgr.Set(ShaderType::LineStripVS);
	myShaderMgr.Set(ShaderType::LineStripPS);
	DX11Interface::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	DX11Interface::context->Draw(5, 0);
}
