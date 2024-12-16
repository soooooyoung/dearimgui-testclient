#include "NetworkClient.h"
#include "DirectWindow.h"
#include "NetworkPacket.h"
#include "Command.h"

static UINT  g_ResizeWidth = 0, g_ResizeHeight = 0;

DirectWindow::DirectWindow()
{
	mRenderer = new D3D11Render();
}

DirectWindow::~DirectWindow()
{
	_CleanupImGui();
	mRenderer->CleanupDeviceD3D();

	::DestroyWindow(mHwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

bool DirectWindow::Initialize()
{
	if (false == _InitializeImGui())
		return false;

	return true;
}

bool DirectWindow::_InitializeImGui()
{
	if (nullptr == mRenderer)
		return false;

	wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
	mHwnd = ::CreateWindowW(wc.lpszClassName, L"Shana's Test Client", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

	// Initialize Direct3D
	if (false == mRenderer->CreateDeviceD3D(mHwnd))
	{
		mRenderer->CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return false;
	}

	// Show the window
	::ShowWindow(mHwnd, SW_SHOWDEFAULT);
	::UpdateWindow(mHwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.ConfigViewportsNoAutoMerge = true;

	// Setup Font
	io.Fonts->Clear();
	ImFont* font = io.Fonts->AddFontFromFileTTF("fonts/hoon.ttf", 16.0f);

	// Setup Dear ImGui style
	SetStyle();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(mHwnd);
	ImGui_ImplDX11_Init(mRenderer->pd3dDevice.Get(), mRenderer->pd3dDeviceContext.Get());

	return true;
}

void DirectWindow::_CleanupImGui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void DirectWindow::MainUI()
{
	const char* window_title = "Main Window";

	ImGuiIO& io = ImGui::GetIO();
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID); // this displays the window correctly during drag

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

	ImGui::Begin(window_title,
		NULL,
		window_flags
	);
	ImGui::PopStyleVar(2);
	ImVec2 button_size = ImVec2(ImGui::GetTextLineHeightWithSpacing(),
		ImGui::GetTextLineHeightWithSpacing());

	if (ImGui::Button("Add Client", ImVec2(100 - ImGui::GetStyle().ItemSpacing.x, 0)))
	{
		if (mCommandCallback)
		{
			mCommandCallback(Command(CommandType::Connect, nullptr));
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Start Test", ImVec2(100 - ImGui::GetStyle().ItemSpacing.x, 0)))
	{
		if (mCommandCallback)
		{
			mCommandCallback(Command(CommandType::StartTest, nullptr));
		}
	}

	if (ImGui::Button("Stop Test", ImVec2(100 - ImGui::GetStyle().ItemSpacing.x, 0)))
	{
		if (mCommandCallback)
		{
			mCommandCallback(Command(CommandType::StopTest, nullptr));
		}
	}

	ImGui::End();
}

bool DirectWindow::ClientWindow(std::weak_ptr<NetworkClient> client)
{
	if (client.expired())
	{
		return false;
	}

	auto clientPtr = client.lock();

	if (!clientPtr->IsConnected())
	{
		return false;
	}

	ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_FirstUseEver);

	auto clientSession = std::to_string(clientPtr->GetSessionID());
    std::string windowTitleStr = "##Client " + clientSession;
    const char* windowTitle = windowTitleStr.c_str();
    std::string inputTitleStr = "##Input " + clientSession;
    const char* inputTitle = inputTitleStr.c_str();

	ImGui::Begin(windowTitle);

	ImVec2 contentSize = ImGui::GetContentRegionAvail();
	if (contentSize.y < 500) contentSize.y = 500;
	{

		if (ImGui::BeginChild("ChatBox", ImVec2(contentSize.x, contentSize.y - 25), true))
		{
			// Chat Box
			ImGui::Text("Chat Box");
			ImGui::Separator();

			if (clientPtr->IsPacketAvailable())
			{
				auto packet = clientPtr->GetPacket();
				std::string chat = std::string((char*)packet->GetBody(), packet->GetBodySize());
				
				clientPtr->PushChat(chat);
			}

			for (auto& chat : clientPtr->GetChatHistory())
			{
				ImGui::Text(chat.c_str());
			}

			ImGui::EndChild();
		}

	}

	auto& sendContext = clientPtr->GetSendContext();

	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 100);
	if (ImGui::InputText(inputTitle,
		(char*)sendContext->GetWriteBuffer(),
		sendContext->GetRemainSize(),
		ImGuiInputTextFlags_EnterReturnsTrue))
	{
		clientPtr->Send();

		ImGui::SetKeyboardFocusHere(-1);	// Auto focus on the next widget
	}

	ImGui::PopItemWidth();

	ImGui::SameLine();

	if (ImGui::Button("Send", ImVec2(100 - ImGui::GetStyle().ItemSpacing.x, 0)))
	{
		clientPtr->Send();
	}

	ImGui::End();

	return true;
}

void DirectWindow::SetStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.ChildRounding = 4.0f;
	style.FrameBorderSize = 1.0f;
	style.FrameRounding = 2.0f;
	style.GrabMinSize = 7.0f;
	style.PopupRounding = 2.0f;
	style.ScrollbarRounding = 12.0f;
	style.ScrollbarSize = 13.0f;
	style.TabBorderSize = 1.0f;
	style.TabRounding = 0.0f;
	style.WindowRounding = 4.0f;
	style.CellPadding = ImVec2(0, 20);

	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
	colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
	colors[ImGuiCol_TabActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_DockingPreview] = ImVec4(1.000f, 0.391f, 0.000f, 0.781f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

}

void DirectWindow::Draw()
{
	MSG msg;
	while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
	{
		mRenderer->Resize(g_ResizeWidth, g_ResizeHeight);
		g_ResizeWidth = g_ResizeHeight = 0;
		mRenderer->CreateRenderTarget();
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Main UI
	MainUI();

	if (!mClientList.empty())
	{
		for (auto iter = mClientList.begin(); iter != mClientList.end();)
		{
			if (!ClientWindow(*iter))
			{
				iter = mClientList.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	// Rendering
	ImGui::Render();
	const float clear_color_with_alpha[4] = { mClearColor.x * mClearColor.w, mClearColor.y * mClearColor.w, mClearColor.z * mClearColor.w, mClearColor.w };
	mRenderer->Render(clear_color_with_alpha);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	mRenderer->pSwapChain->Present(1, 0); // Present with vsync
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	DirectWindow* window = reinterpret_cast<DirectWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));


	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;

		g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			const RECT* suggested_rect = (RECT*)lParam;
			::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
