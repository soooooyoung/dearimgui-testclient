#pragma once

// WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

// STANDARD LIBRARIES
#include <string>
#include <thread>
#include <vector>
#include <chrono>

// DIRECTX
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#pragma comment(lib, "Ws2_32.lib")

// DEAR IMGUI
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

