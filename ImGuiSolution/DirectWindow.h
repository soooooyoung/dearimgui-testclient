#include <wtypes.h>
#include <d3d11.h>
#include <tchar.h>
#include <functional>
#include <string>
#include <memory>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Command.h"
#include <vector>

class D3D11Render;
class DirectWindow
{
public:
	DirectWindow();
	~DirectWindow();

	bool Initialize();
	void RunLoop();

	void MainUI();
	void ClientUI();
	void SetStyle();

	void PushChat(const std::string& chat)
	{
		mChatHistory.push_back(chat);
	}

	D3D11Render* mRenderer;
	std::function<void(Command&&)> mCommandCallback;
private:
	bool _InitializeImGui();
	void _CleanupImGui();

	bool mShowMainUI = true;

	HWND mHwnd = nullptr;
	WNDCLASSEXW wc = {};

	std::vector<std::string> mChatHistory;
};

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);