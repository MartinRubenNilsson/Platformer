#include "pch.h"
#include "App.h"


int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
    App app;
    if (!app.Init())
        return EXIT_FAILURE;
    while (app.Run()) {}
    return app.GetExitCode();
}


// Constructor

App::App()
    : myWindow()
    , myEngine(myWindow)
    , myGame(myEngine)
	, myExitCode(0)
{
}


// Public methods

bool App::Init()
{
    fs::path path = GetPath();
    fs::current_path(path.remove_filename());

    if (!myWindow.Init())
        return false;
    
    if (!myEngine.Init())
        return false;

    if (!myGame.Init())
        return false;

    myWindow.Show();

    return true;
}

bool App::Run()
{
    MSG msg = {};

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            myExitCode = (int)msg.wParam;
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    myEngine.NewFrame();
    myGame.Update();
    myEngine.EndFrame();

    return true;
}


// Getters

fs::path App::GetPath() const
{
    wchar_t fileName[MAX_PATH];
    GetModuleFileName(NULL, fileName, MAX_PATH);
    return fileName;
}
