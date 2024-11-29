#pragma once

namespace MainWindow {
	static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
	static bool                     g_SwapChainOccluded = false;

	class DirectWindow
	{
	public:
		DirectWindow();
		~DirectWindow();

		bool Initialize();
		bool InitializeImGui();
		void Destroy();

		bool CreateDeviceD3D();
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();

		void RenderUI();

		HWND GetWindowHwnd() const { return WindowHwnd; }

		WNDPROC								m_OldWndProc{};

		ID3D11Device* mDevice{};
		ID3D11DeviceContext* mDeviceContext{};
		ID3D11RenderTargetView* mRenderTargetView{};
		IDXGISwapChain* mpSwapChain{};
	private:
		static LRESULT APIENTRY				WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		WNDCLASSEXW							WindowClass;
		HWND								WindowHwnd;

		bool CreateDirectWindow();
	};
}