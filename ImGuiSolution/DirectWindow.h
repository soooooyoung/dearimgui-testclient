#pragma once
#include "pch.h"

namespace MainWindow {
	class DirectWindow
	{
	public:
		DirectWindow();
		~DirectWindow();

		bool Initialize();
		void Destroy();

		bool CreateDeviceD3D();
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();

		HWND GetWindowHwnd() const { return WindowHwnd; }

		WNDPROC								m_OldWndProc{};
	private:
		static LRESULT APIENTRY				WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		WNDCLASSEXW							WindowClass;
		HWND								WindowHwnd;


		ID3D11Device* m_Device{};
		ID3D11DeviceContext* m_DeviceContext{};
		ID3D11RenderTargetView* m_RenderTargetView{};
		IDXGISwapChain* m_pSwapChain{};
	};
}