#pragma once
#include "window/MessageHandler.h"
#include "math/vec2.hpp"
#include "math/Color.h"
#include <memory>

class BackBuffer;
class StateManager;
class ShaderManager;
class ConstantBuffer;
class Drawer;
class Postprocessor;
class Transitioner;


class Graphics : public MessageHandler
{
public:
	Graphics();
	~Graphics();

	bool Init();
	bool Handle(const WindowMessage&) override;
	void NewFrame();
	void EndFrame();

	BackBuffer& GetBackBuffer() { return *myBackBuffer; }
	StateManager& GetStateMgr() { return *myStateMgr; }
	ShaderManager& GetShaderMgr() { return *myShaderMgr; }

	ConstantBuffer& GetCameraBuffer() { return *myCameraBuffer; }

	Drawer& GetDrawer() { return *myDrawer; }
	Postprocessor& GetPostprocessor() { return *myPostprocessor; }
	Transitioner& GetTransitioner() { return *myTransitioner; }

private:
	Color myClearColor;

	std::unique_ptr<BackBuffer> myBackBuffer;
	std::unique_ptr<StateManager> myStateMgr;
	std::unique_ptr<ShaderManager> myShaderMgr;

	std::unique_ptr<ConstantBuffer> myCameraBuffer;

	std::unique_ptr<Drawer> myDrawer;
	std::unique_ptr<Postprocessor> myPostprocessor;
	std::unique_ptr<Transitioner> myTransitioner;
};