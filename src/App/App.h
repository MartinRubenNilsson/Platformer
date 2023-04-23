#pragma once
#include "window/Window.h"
#include "Engine.h"
#include "Game.h"
#include <filesystem>

namespace fs = std::filesystem;


class App
{
public:
	App();

	bool Init();
	bool Run();

	fs::path GetPath() const;
	int GetExitCode() const { return myExitCode; }

private:
	Window myWindow;
	Engine myEngine;
	Game myGame;
	int myExitCode;
};