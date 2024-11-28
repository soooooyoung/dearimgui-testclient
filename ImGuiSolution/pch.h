#pragma once

// WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>

// DEAR IMGUI
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// DIRECTX
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

// STANDARD LIBRARIES
#include <string>
#include <thread>
#include <vector>
#include <chrono>


static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;

