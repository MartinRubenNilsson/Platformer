#include "pch.h"
#include "graphics/Graphics.h"
#include "graphics/texture/BackBuffer.h"
#include "graphics/state/StateManager.h"
#include "graphics/shader/ShaderManager.h"
#include "graphics/cbuffer/ConstantBuffer.h"
#include "graphics/cbuffer/CameraData.h"
#include "graphics/renderer/Drawer.h"
#include "graphics/renderer/Postprocessor.h"
#include "graphics/renderer/Transitioner.h"
#include "window/Window.h"
#include "window/WindowMessage.h"


// Constructor & destructor

Graphics::Graphics()
	: myClearColor{ 0.f, 0.f, 0.f, 1.f }
	, myBackBuffer(new BackBuffer())
	, myCameraBuffer(new ConstantBuffer())
	, myStateMgr(new StateManager())
	, myShaderMgr(new ShaderManager())
	, myDrawer(new Drawer(*this))
	, myPostprocessor(new Postprocessor(*this))
	, myTransitioner(new Transitioner(*this))
{
}

Graphics::~Graphics() = default;


// Public methods

bool Graphics::Init()
{
	if (!myBackBuffer->Init())
		return false;

	if (!myStateMgr->Init())
		return false;

	if (!myShaderMgr->Init())
		return false;

	if (!myCameraBuffer->Create(sizeof(CameraData)))
		return false;
	
	if (!myDrawer->Init())
		return false;

	if (!myPostprocessor->Init())
		return false;

	if (!myTransitioner->Init())
		return false;

	return true;
}

bool Graphics::Handle(const WindowMessage& aMessage)
{
	switch (aMessage.type)
	{
	case MessageType::SysKeyDown:
		if (aMessage.key != Key::Return)
			return false;
		[[fallthrough]];
	case MessageType::ExitSizeMove:
		myBackBuffer->Resize(aMessage.window.GetClientSize());
		return true;
	default:
		return false;
	}
}

void Graphics::NewFrame()
{
	myBackBuffer->Clear(myClearColor);
	myBackBuffer->SetAsTarget();
	myStateMgr->Set(BlendState::Enabled);
	myCameraBuffer->SetForVS(CBUFFER_SLOT_CAMERA);
}

void Graphics::EndFrame()
{
	myBackBuffer->Present();
}
