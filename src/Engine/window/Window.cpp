#include "pch.h"
#include "window/Window.h"
#include "window/WindowMessage.h"
#include "window/MessageHandler.h"


namespace
{
    Window* window = nullptr;

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        WindowMessage message = { *window, msg, wParam, lParam };

        switch (msg)
        {
        case WM_ACTIVATE:
            message.active = static_cast<bool>(LOWORD(wParam));
            message.minimized = static_cast<bool>(HIWORD(wParam));
            break;
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        {
            WORD keyCode = LOWORD(wParam);
            WORD keyFlags = HIWORD(lParam);
            WORD repeatCount = LOWORD(lParam);

            //BOOL isKeyUp = (keyFlags & KF_UP) == KF_UP;
            //BOOL wasKeyDown = (keyFlags & KF_REPEAT) == KF_REPEAT;
            BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;

            WORD scanCode = LOBYTE(keyFlags);
            if (isExtendedKey)
                scanCode = MAKEWORD(scanCode, 0xE0);

            switch (keyCode)
            {
            case VK_SHIFT:   // converts to VK_LSHIFT or VK_RSHIFT
            case VK_CONTROL: // converts to VK_LCONTROL or VK_RCONTROL
            case VK_MENU:    // converts to VK_LMENU or VK_RMENU
                keyCode = LOWORD(MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX));
                break;
            }

            message.key = static_cast<Key>(keyCode);

            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        if (window->Send(message))
            return 0;

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}


// Constructor & destructor

Window::Window()
    : myHandle(nullptr)
    , myClassName(L"Window")
    , myTitle(L"Platformer")
{
    window = this;
}


// Public methods

bool Window::Init()
{
    if (!Register())
        return false;

    if (!Create())
        return false;

    return true;
}

void Window::Show()
{
    ShowWindow(static_cast<HWND>(myHandle), SW_SHOWNORMAL);
}


// Getters

uint2 Window::GetClientSize() const
{
    RECT rect = {};
    GetClientRect(static_cast<HWND>(myHandle), &rect);
    return {
        static_cast<unsigned>(rect.right - rect.left),
        static_cast<unsigned>(rect.bottom - rect.top)
    };
}



void Window::Connect(MessageHandler* aHandler)
{
    if (rn::count(myHandlers, aHandler) == 0)
        myHandlers.push_back(aHandler);
}

void Window::Disconnect(MessageHandler* aHandler)
{
    auto toErase = rn::remove(myHandlers, aHandler);
    myHandlers.erase(toErase.begin(), toErase.end());
}

bool Window::Send(const WindowMessage& aMessage)
{
    for (auto handler : myHandlers)
    {
        if (handler->Handle(aMessage))
            return true;
    }

    return false;
}


// Private methods

bool Window::Register()
{
    WNDCLASSEXW wndClass = {};
    wndClass.cbSize         = sizeof(WNDCLASSEX);
    wndClass.lpfnWndProc    = WndProc;
    wndClass.hInstance      = GetModuleHandle(NULL);
    wndClass.lpszClassName  = myClassName;

    return RegisterClassEx(&wndClass);
}

bool Window::Create()
{
    myHandle = CreateWindow(
        myClassName,
        myTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0,
        CW_USEDEFAULT, 0,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    return myHandle;
}
