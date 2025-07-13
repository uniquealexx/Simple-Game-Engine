#include "DX11Renderer.hpp"
#include "Shader.hpp"
#include "VertexBuffer.hpp"
#include "Primitive2D.hpp"

DX11Renderer::DX11Renderer() = default;

DX11Renderer::~DX11Renderer() {
    Shutdown();
}

bool DX11Renderer::Initialize(HWND hwnd, int width, int height) {
    this->width = width;
    this->height = height;

    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 2;
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &scd,
        swapChain.GetAddressOf(),
        device.GetAddressOf(),
        nullptr,
        context.GetAddressOf()
    );

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());

    shader = std::make_unique<Shader>();
    if (!shader->Load(device.Get(), IDR_SHADER_VS, IDR_SHADER_PS)) {
        MessageBox(hwnd, L"Failed to load shaders", L"Error", MB_OK);
        return false;
    }

    AddPrimitive(std::make_unique<Triangle>());
    AddPrimitive(std::make_unique<Quad>());

    return true;
}

void DX11Renderer::AddPrimitive(std::unique_ptr<Primitive2D> primitive) {
    primitives.push_back(std::move(primitive));

    auto& prim = primitives.back();
    auto vb = std::make_unique<VertexBuffer>();
    vb->Create(
        device.Get(),
        prim->GetVertices(),
        sizeof(Vertex),
        prim->GetVertexCount()
    );

    vertexBuffers.push_back(std::move(vb));
}

void DX11Renderer::RenderFrame() {
    const float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    context->ClearRenderTargetView(renderTargetView.Get(), clearColor);
    context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context->RSSetViewports(1, &viewport);

    shader->Bind(context.Get());

    for (size_t i = 0; i < primitives.size(); ++i) {
        context->IASetPrimitiveTopology(primitives[i]->GetTopology());
        vertexBuffers[i]->Bind(context.Get());
        context->Draw(vertexBuffers[i]->GetVertexCount(), 0);
    }

    swapChain->Present(1, 0);
}

void DX11Renderer::Shutdown() {
    renderTargetView.Reset();
    swapChain.Reset();
    context.Reset();
    device.Reset();
    vertexBuffers.clear();
    primitives.clear();
}