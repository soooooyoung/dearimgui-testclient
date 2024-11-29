#include "pch.h"
#include "DirectWindow.h"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace MainWindow {

    DirectWindow::DirectWindow()
    {
    }

    DirectWindow::~DirectWindow()
    {
    }

    bool DirectWindow::Initialize()
    {
		if (!CreateDirectWindow())
		{
			return false;
		}

        if (!CreateDeviceD3D())
        {
			CleanupDeviceD3D();
			::UnregisterClassW(WindowClass.lpszClassName, WindowClass.hInstance);
			return false;
        }

        ::ShowWindow(WindowHwnd, SW_SHOWDEFAULT);
        ::UpdateWindow(WindowHwnd);

        return true;
    }

    void DirectWindow::Destroy()
    {
        ::DestroyWindow(WindowHwnd);
        ::UnregisterClassW(WindowClass.lpszClassName, WindowClass.hInstance);
    }

    bool DirectWindow::CreateDeviceD3D()
    {
        // Setup swap chain
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = WindowHwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags = 0;
        //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
        HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &mpSwapChain, &mDevice, &featureLevel, &mDeviceContext);
        if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
            res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &mpSwapChain, &mDevice, &featureLevel, &mDeviceContext);
        if (res != S_OK)
            return false;

        CreateRenderTarget();
        return true;
    }

    void DirectWindow::CleanupDeviceD3D()
    {
        CleanupRenderTarget();
        if (mpSwapChain) { mpSwapChain->Release(); mpSwapChain = nullptr; }
        if (mDeviceContext) { mDeviceContext->Release(); mDeviceContext = nullptr; }
        if (mDevice) { mDevice->Release(); mDevice = nullptr; }
    }

    void DirectWindow::CreateRenderTarget()
    {
        ID3D11Texture2D* pBackBuffer;
        mpSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        mDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mRenderTargetView);
        pBackBuffer->Release();
    }

    void DirectWindow::CleanupRenderTarget()
    {
        if (mRenderTargetView) { mRenderTargetView->Release(); mRenderTargetView = nullptr; }
    }

    void DirectWindow::RenderUI()
    {

        if (g_SwapChainOccluded && mpSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            return;
        }
        g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            mpSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Hello, World", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        ImGui::Text("This is some useful text.");

        ImGui::End();

        ImGui::EndFrame();
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);
        mDeviceContext->ClearRenderTargetView(mRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    bool DirectWindow::CreateDirectWindow()
    {
        WindowClass.cbSize = sizeof(WNDCLASSEX);
        WindowClass.style = CS_CLASSDC;
        WindowClass.lpfnWndProc = WndProc;
        WindowClass.cbClsExtra = 0;
        WindowClass.cbWndExtra = 0;
        WindowClass.hInstance = GetModuleHandle(nullptr);
        WindowClass.hIcon = nullptr;
        WindowClass.hCursor = nullptr;
        WindowClass.hbrBackground = nullptr;
        WindowClass.lpszMenuName = nullptr;
        WindowClass.lpszClassName = L"ImGui Test Client";
        WindowClass.hIconSm = nullptr;
        ::RegisterClassExW(&WindowClass);

        WindowHwnd = ::CreateWindow(WindowClass.lpszClassName, L"ImGui Test Client", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, WindowClass.hInstance, nullptr);

        return true;
    }
    bool DirectWindow::InitializeImGui()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(WindowHwnd);
        ImGui_ImplDX11_Init(mDevice, mDeviceContext);

		return true;
    }

    LRESULT WINAPI DirectWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
                return 0;
            g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
            g_ResizeHeight = (UINT)HIWORD(lParam);
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        case WM_DPICHANGED:
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
            {
                //const int dpi = HIWORD(wParam);
                //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
                const RECT* suggested_rect = (RECT*)lParam;
                ::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
            }
            break;
        }
        return ::DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}

