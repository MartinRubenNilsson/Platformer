#include "pch.h"
#include "interface/ImGuiInterface.h"
#include "window/Window.h"
#include "window/WindowMessage.h"
#include "interface/DX11Interface.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// Destructor

ImGuiInterface::ImGuiInterface() = default;

ImGuiInterface::~ImGuiInterface()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


// Public methods

bool ImGuiInterface::Init(void* hWnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	if (!ImGui_ImplWin32_Init(hWnd))
		return false;

	if (!ImGui_ImplDX11_Init(DX11Interface::device, DX11Interface::context))
		return false;

	return true;
}

bool ImGuiInterface::Handle(const WindowMessage& aMessage)
{
	return ImGui_ImplWin32_WndProcHandler(
		(HWND)aMessage.window.GetHandle(), 
		aMessage.msg, aMessage.wParam, aMessage.lParam
	);
}

void ImGuiInterface::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiInterface::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
