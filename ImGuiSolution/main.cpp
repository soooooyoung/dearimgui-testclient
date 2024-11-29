#include "pch.h"

#include "DirectWindow.h"
#include <tchar.h>


bool isRunning = true;
inline std::unique_ptr<MainWindow::DirectWindow> g_D3D11Window = std::make_unique<MainWindow::DirectWindow>();

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
        return false;
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
        return false;
    }

    ret = connect(ConnectSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));
    if (ret == SOCKET_ERROR) {
        std::cerr << "Connect failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(ConnectSocket);
        WSACleanup();
        return false;
    }

    freeaddrinfo(result);
    std::cout << "Successfully connected to the server." << std::endl;

    return true;
}

// Main code
int main(int, char**)
{
	// Initialize WinSock
    //if (!InitializeSocket()) {
    //    return 1;
    //}

	// Create application window
	if (!g_D3D11Window->Initialize())
	{
		return 1;
	}

    // Setup Dear ImGui context
	if (!g_D3D11Window->InitializeImGui())
	{
		return 1;
	}

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

		g_D3D11Window->RenderUI();

        // Present
        HRESULT hr = g_D3D11Window->mpSwapChain->Present(1, 0);   // Present with vsync
        //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
        MainWindow::g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    g_D3D11Window->CleanupDeviceD3D();
	g_D3D11Window->Destroy();

    return 0;
}
