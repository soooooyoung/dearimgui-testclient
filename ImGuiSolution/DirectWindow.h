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

class NetworkClient;
class DirectWindow
{
public:
	DirectWindow();
	~DirectWindow();

	bool Initialize();
	//void RunLoop();

	void Draw();

	void MainUI();
	void ClientWindow(std::weak_ptr<NetworkClient> client);
	void SetStyle();

	void PushChat(const std::string& chat)
	{
		mChatHistory.push_back(chat);
	}

	void PushClient(std::weak_ptr<NetworkClient> client)
	{
		mClientList.push_back(client);
	}

	D3D11Render* mRenderer;
	std::function<void(Command&&)> mCommandCallback;
private:
	bool _InitializeImGui();
	void _CleanupImGui();

	bool mShowMainUI = true;

	HWND mHwnd = nullptr;
	WNDCLASSEXW wc = {};

	std::vector<std::weak_ptr<NetworkClient>> mClientList;
	std::vector<std::string> mChatHistory;

	ImVec4 mClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);