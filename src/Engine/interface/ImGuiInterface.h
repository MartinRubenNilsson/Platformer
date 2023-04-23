#pragma once
#include "window/MessageHandler.h"


class ImGuiInterface : public MessageHandler
{
public:
	ImGuiInterface();
	~ImGuiInterface();

	bool Init(void* hWnd);
	bool Handle(const WindowMessage&) override;
	void NewFrame();
	void Render();
};