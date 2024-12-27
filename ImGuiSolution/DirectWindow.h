#pragma once
#include <tchar.h>
#include <functional>
#include <string>
#include <memory>
#include <wchar.h>
#include <vector>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Command.h"
#include "D3D11Render.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class NetworkClient;
class DirectWindow
{
public:
	DirectWindow();
	~DirectWindow();

	bool Initialize();

	void Draw();

	void MainUI();
	bool ClientWindow(std::weak_ptr<NetworkClient> client);
	void SetStyle();

	void PushClient(std::weak_ptr<NetworkClient> client)
	{
		mClientList.push_back(client);
	}

	D3D11Render* mRenderer;
	std::function<void(Command&&)> mCommandCallback;

	UINT mResizeWidth = 0;
	UINT mResizeHeight = 0;

private:
	bool _InitializeImGui();
	void _CleanupImGui();

	bool mShowMainUI = true;

	HWND mHwnd = nullptr;
	WNDCLASSEXW wc = {};

	std::vector<std::weak_ptr<NetworkClient>> mClientList;

	ImVec4 mClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};