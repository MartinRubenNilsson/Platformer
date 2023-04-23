#pragma once

struct WindowMessage;


class MessageHandler
{
public:
	virtual bool Handle(const WindowMessage&) = 0;
};