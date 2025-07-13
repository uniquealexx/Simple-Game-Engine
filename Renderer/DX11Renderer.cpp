#include "DX11Renderer.hpp"
#include "Shader.hpp"
#include "VertexBuffer.hpp"
#include "Primitive2D.hpp"

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

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(ConstantBuffer);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device->CreateBuffer(&cbDesc, nullptr, &constantBuffer);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTexture;
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    device->CreateTexture2D(&descDepth, nullptr, &depthStencilTexture);
    device->CreateDepthStencilView(depthStencilTexture.Get(), nullptr, &depthStencilView);

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    device->CreateDepthStencilState(&dsDesc, &depthStencilState);

    viewMatrix = DirectX::XMMatrixLookAtLH(
        { 0.0f, 0.0f, -5.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f }
    );

    projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
        DirectX::XM_PIDIV2,
        static_cast<float>(width) / height,
        0.01f,
        100.0f
    );

    orthoMatrix = DirectX::XMMatrixOrthographicLH(
        static_cast<float>(width),
        static_cast<float>(height),
        0.0f,
        1.0f
    );

    return true;
}

void DX11Renderer::AddPrimitive(std::unique_ptr<Primitive2D> primitive) {
    primitives.push_back(std::move(primitive));
    auto& prim = primitives.back();

    auto vb = std::make_unique<VertexBuffer>();
    vb->Create(device.Get(), prim->GetVertices(), sizeof(Vertex), prim->GetVertexCount());

    if (prim->Is3D()) {
        if (auto cube = dynamic_cast<Cube*>(prim.get())) {
            vb->CreateIndexBuffer(device.Get(), cube->GetIndices(), cube->GetIndexCount());
        }
    }
    else {
        vb->CreateIndexBuffer(device.Get(), nullptr, 0); 
    }

    vertexBuffers.push_back(std::move(vb));
}

void DX11Renderer::RenderFrame() {
    const float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    context->ClearRenderTargetView(renderTargetView.Get(), clearColor);
    context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context->RSSetViewports(1, &viewport);

    shader->Bind(context.Get());

    context->OMSetDepthStencilState(depthStencilState.Get(), 0);

    static float angle = 0.0f;
    angle += 0.01f;
    worldMatrix = DirectX::XMMatrixRotationY(angle);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    ConstantBuffer* cb = static_cast<ConstantBuffer*>(mappedResource.pData);
    cb->world = XMMatrixTranspose(worldMatrix);
    cb->view = XMMatrixTranspose(viewMatrix);
    cb->projection = XMMatrixTranspose(projectionMatrix);
    context->Unmap(constantBuffer.Get(), 0);
    context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    for (size_t i = 0; i < primitives.size(); ++i) {
        if (!primitives[i]->Is3D()) continue;

        context->IASetPrimitiveTopology(primitives[i]->GetTopology());
        vertexBuffers[i]->Bind(context.Get());

        if (auto cube = dynamic_cast<Cube*>(primitives[i].get())) {
            vertexBuffers[i]->BindIndexBuffer(context.Get());
            context->DrawIndexed(vertexBuffers[i]->GetIndexCount(), 0, 0);
        }
        else {
            context->Draw(vertexBuffers[i]->GetVertexCount(), 0);
        }
    }

    context->OMSetDepthStencilState(nullptr, 0);

    ID3D11Buffer* nullBuffer = nullptr;
    UINT stride = 0;
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);
    context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

    context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    cb = static_cast<ConstantBuffer*>(mappedResource.pData);
    cb->world = XMMatrixTranspose(DirectX::XMMatrixIdentity());
    cb->view = XMMatrixTranspose(DirectX::XMMatrixIdentity());
    cb->projection = XMMatrixTranspose(orthoMatrix);
    context->Unmap(constantBuffer.Get(), 0);
    context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    for (size_t i = 0; i < primitives.size(); ++i) {
        if (primitives[i]->Is3D()) continue;

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
    depthStencilView.Reset();
    depthStencilState.Reset();
    constantBuffer.Reset();
}