#include <wtypes.h>
#include <d3d11.h>
#include <tchar.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"


class D3D11Render;
class DirectWindow
{
public:
	DirectWindow();
	~DirectWindow();

	bool Initialize();
	void RunLoop();

	D3D11Render* mRenderer;
private:
	bool InitializeImGui();
	void CleanupImGui();

	bool mShowDemoWindow = true;
	bool mShowAnotherWindow = true;

	HWND mHwnd = nullptr;
	WNDCLASSEXW wc = {};
};

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);