# Test Client

This project is a test client using [Dear ImGui library](https://github.com/ocornut/imgui), specifically meant to test servers in [this repository](https://github.com/soooooyoung/iocp-study).

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
