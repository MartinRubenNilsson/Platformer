#pragma once
#include "window/Key.h"
#include <Windows.h>

enum class MessageType : unsigned
{
	Activate      = 0x0006,
	KeyDown       = 0x0100,
	KeyUp         = 0x0101,
	SysKeyDown    = 0x0104,
	SysKeyUp      = 0x0105,
	EnterSizeMove = 0x0231,
	ExitSizeMove  = 0x0232
};

struct WindowMessage
{
	class Window& window;

	union
	{
		UINT msg;
		MessageType type;
	};

	WPARAM wParam;
	LPARAM lParam;

	union
	{
		Key key;
		bool active;
	};

	union
	{
		bool minimized;
	};
};
