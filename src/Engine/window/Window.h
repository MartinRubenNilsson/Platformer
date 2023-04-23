#pragma once
#include "math/vec2.hpp"
#include <vector>

class MessageHandler;
struct WindowMessage;


class Window
{
public:
	Window();

	bool Init();
	void Show();

	void* GetHandle() const { return myHandle; }
	uint2 GetClientSize() const;

	void Connect(MessageHandler*);
	void Disconnect(MessageHandler*);
	bool Send(const WindowMessage&);

private:
	bool Register();
	bool Create();

	void* myHandle;
	const wchar_t* myClassName;
	const wchar_t* myTitle;

	std::vector<MessageHandler*> myHandlers;
};