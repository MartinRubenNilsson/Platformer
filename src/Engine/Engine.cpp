#include "pch.h"
#include "Engine.h"
#include "window/Window.h"
#include "interface/DX11Interface.h"
#include "interface/ImGuiInterface.h"
#include "interface/FMODInterface.h"
#include "clock/Clock.h"
#include "input/Input.h"
#include "graphics/Graphics.h"


// Destructor

Engine::Engine(Window& aWindow)
	: myWindow(aWindow)
	, myDX11(new DX11Interface())
	, myImGui(new ImGuiInterface())
	, myFMOD(new FMODInterface())
	, myInput(new Input())
	, myClock(new Clock())
	, myGraphics(new Graphics())
{
}

Engine::~Engine()
{
	myWindow.Disconnect(myGraphics.get());
	myWindow.Disconnect(myClock.get());
	myWindow.Disconnect(myInput.get());
	myWindow.Disconnect(myImGui.get());
}


// Public methods

bool Engine::Init()
{
	if (!myDX11->Init(myWindow.GetHandle()))
		return false;

	if (!myImGui->Init(myWindow.GetHandle()))
		return false;

	if (!myFMOD->Init())
		return false;

	if (!myGraphics->Init())
		return false;

	myWindow.Connect(myImGui.get());
	myWindow.Connect(myInput.get());
	myWindow.Connect(myClock.get());
	myWindow.Connect(myGraphics.get());

	return true;
}

void Engine::NewFrame()
{
	myInput->NewFrame();
	myImGui->NewFrame();
	myGraphics->NewFrame();
	myClock->Update();
	myFMOD->Update();
}

void Engine::EndFrame()
{
	myFMOD->ImGuiEditor();
	myImGui->Render();
	myGraphics->EndFrame();
	myInput->EndFrame();
}
