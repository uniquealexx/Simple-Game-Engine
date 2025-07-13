#pragma once

#include "Include.hpp"
#include "Shader.hpp"
#include "VertexBuffer.hpp"
#include "Primitive2D.hpp"

struct ConstantBuffer {
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
};

class DX11Renderer {
public:
    DX11Renderer() = default;
    ~DX11Renderer() { Shutdown(); }

    bool Initialize(HWND hwnd, int width, int height);
    void AddPrimitive(std::unique_ptr<Primitive2D> primitive);
    void RenderFrame();
    void Shutdown();

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

    std::unique_ptr<Shader> shader;
    std::vector<std::unique_ptr<Primitive2D>> primitives;
    std::vector<std::unique_ptr<VertexBuffer>> vertexBuffers;

    int width = 0;
    int height = 0;

    DirectX::XMMATRIX worldMatrix;
    DirectX::XMMATRIX viewMatrix;
    DirectX::XMMATRIX projectionMatrix;
    DirectX::XMMATRIX orthoMatrix;
};