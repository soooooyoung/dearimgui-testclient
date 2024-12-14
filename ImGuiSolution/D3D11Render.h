#pragma once

#include <d3d11.h>
#include <wrl.h>

class D3D11Render {
public:
    bool CreateDeviceD3D(HWND hwnd);
    void CleanupDeviceD3D();
    void Resize(UINT width, UINT height);
    void Render(const float clear_color[4]);
    void CreateRenderTarget();
    void CleanupRenderTarget();

    Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pd3dDeviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pMainRenderTargetView;
};
