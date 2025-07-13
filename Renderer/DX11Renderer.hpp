#pragma once

#include "Include.hpp"

class Shader;
class VertexBuffer;
class Primitive2D;

class DX11Renderer {
public:
    DX11Renderer();
    ~DX11Renderer();

    bool Initialize(HWND hwnd, int width, int height);
    void RenderFrame();
    void Shutdown();

    void AddPrimitive(std::unique_ptr<Primitive2D> primitive);

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

    std::unique_ptr<Shader> shader;
    std::vector<std::unique_ptr<VertexBuffer>> vertexBuffers;
    std::vector<std::unique_ptr<Primitive2D>> primitives;

    int width = 0;
    int height = 0;
};