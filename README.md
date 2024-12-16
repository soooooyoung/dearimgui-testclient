# Test Client

[![en](https://img.shields.io/badge/lang-english-yellow.svg)](README.md)
[![kr](https://img.shields.io/badge/lang-한국어-red.svg)](README.kr.md)

<div style="display:flex; justify-content:center" >
<image src="images/minion.png" width=200/>
</div>

This project is a test client using [Dear ImGui library](https://github.com/ocornut/imgui),
specifically meant to test servers in [this repository](https://github.com/soooooyoung/iocp-study).

# Implementation Process

## Setting up Dear ImGUI

> See [examples directory](https://github.com/ocornut/imgui/tree/master/examples) of the official GitHub repository of Dear ImGUI for examples specific to your platform.

Include Dear ImGUI's Implementations for:

- **Win32 API**
- **DirectX11**
- **imgui_stdlib** (`InputText()` wrappers for std::string)
- **imgui.natstepfilter** (support for `Visual Studio Debugger`, disable stepping into trivial functions)
- **imgui.natvis** (support for `Visual Studio Debugger`, describe Dear ImGui types for better display)

ImGUI is small enough to include its files directly.

### [DirectWindow](ImGuiSolution/DirectWindow.h)

`DirectWindow` integrates Dear ImGui's rendering loop and manages the application's main window and UI rendering.

## Integrating Network Operations

Most network operations adhere to general practices from the testing server, leveraging reusable code.

### [Network Manager](/ImGuiSolution/NetworkManager.h)

The NetworkManager class initializes Winsock and creates an I/O Completion Port (IOCP) to enable efficient asynchronous networking.

### [Client Connections](/ImGuiSolution/NetworkClient.h)

The `NetworkClient` instance is added upon `Connect` command. Each handles individual client network operations, utilizing `ConnectEx` for non-blocking connection establishment.

## Command Handling

`DirectWindow` invokes commands via a callback, linking UI actions (e.g., button clicks) to network operations.

[Commands](/ImGuiSolution/Command.h) such as Connect are then queued and processed concurrently on separate thread using [concurrent_queue](https://learn.microsoft.com/en-us/cpp/parallel/concrt/reference/concurrent-queue-class?view=msvc-170).
