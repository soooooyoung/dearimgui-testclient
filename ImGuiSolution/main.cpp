#include "pch.h"
#include "MainWindow.h"
#include "DirectWindow.h"
#include <tchar.h>

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

inline std::unique_ptr<MainWindow::DirectWindow> g_D3D11Window = std::make_unique<MainWindow::DirectWindow>();
bool isRunning = true;


bool InitializeSocket()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        return false;
    }

    // Create socket
    SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    struct addrinfo* result = nullptr;
    struct addrinfo hints = {};

    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_protocol = IPPROTO_TCP; // TCP

    // Replace "127.0.0.1" with the server's IP address or hostname
    auto ret = getaddrinfo("127.0.0.1", "9000", &hints, &result);
    if (ret != 0) {
        std::cerr << "getaddrinfo failed with error: " << ret << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    ret = connect(ConnectSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));
    if (ret == SOCKET_ERROR) {
        std::cerr << "Connect failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    std::cout << "Successfully connected to the server." << std::endl;
}

// Main code
int main(int, char**)
{
	// Initialize WinSock
    if (!InitializeSocket()) {
        return 1;
    }

	// Create application window
	if (!g_D3D11Window->Initialize())
	{
		return 1;
	}

    // Initialize Direct3D
	if (false == g_D3D11Window->CreateDeviceD3D())
    {
        return 1;
    }

    // Show the window
	::ShowWindow(g_D3D11Window->GetWindowHwnd(), SW_SHOWDEFAULT);
    ::UpdateWindow(g_D3D11Window->GetWindowHwnd());

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
    ImGui_ImplWin32_Init(g_D3D11Window->GetWindowHwnd());
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window being minimized or screen locked
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_D3D11Window->CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            g_D3D11Window->CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

		MainWindow::RenderUI();


        // Rendering
		ImGui::EndFrame();
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        // Present
        HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
        //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    g_D3D11Window->CleanupDeviceD3D();
	g_D3D11Window->Destroy();

    return 0;
}
