#pragma once
#include <memory>

class Window;
class Input;
class Clock;
class Graphics;


class Engine
{
public:
	Engine(Window&);
	~Engine();

	bool Init();
	void NewFrame();
	void EndFrame();

	Window& GetWindow() { return myWindow; }
	Input& GetInput() { return *myInput; }
	Clock& GetClock() { return *myClock; }
	Graphics& GetGraphics() { return *myGraphics; }

private:
	Window& myWindow;

	std::unique_ptr<class DX11Interface> myDX11;
	std::unique_ptr<class ImGuiInterface> myImGui;
	std::unique_ptr<class FMODInterface> myFMOD;

	std::unique_ptr<Input> myInput;
	std::unique_ptr<Clock> myClock;
	std::unique_ptr<Graphics> myGraphics;
};