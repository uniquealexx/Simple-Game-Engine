#include "Shader.hpp"

Shader::Shader() {}

Shader::~Shader() {
    if (vertexShader) vertexShader->Release();
    if (pixelShader) pixelShader->Release();
    if (inputLayout) inputLayout->Release();
}

bool Shader::Load(ID3D11Device* device, int vsResourceId, int psResourceId) {
    HRSRC hVsRes = FindResource(nullptr, MAKEINTRESOURCE(vsResourceId), L"HLSL");
    if (!hVsRes) {
        MessageBox(nullptr, L"Vertex shader resource not found", L"Error", MB_OK);
        return false;
    }

    HGLOBAL hVsData = LoadResource(nullptr, hVsRes);
    if (!hVsData) {
        MessageBox(nullptr, L"Failed to load vertex shader resource", L"Error", MB_OK);
        return false;
    }

    DWORD vsSize = SizeofResource(nullptr, hVsRes);
    LPVOID vsData = LockResource(hVsData);

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    UINT compileFlags = 0;
#ifdef _DEBUG
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = D3DCompile(
        vsData,         
        vsSize,         
        nullptr,        
        nullptr,        
        nullptr,        
        "main",         
        "vs_5_0",       
        compileFlags,   
        0,              
        &vsBlob,        
        &errorBlob      
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA("Vertex shader compilation error:\n");
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        UnlockResource(hVsData);
        FreeResource(hVsData);
        return false;
    }

    hr = device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        &vertexShader
    );

    if (FAILED(hr)) {
        vsBlob->Release();
        UnlockResource(hVsData);
        FreeResource(hVsData);
        return false;
    }

    HRSRC hPsRes = FindResource(nullptr, MAKEINTRESOURCE(psResourceId), L"HLSL");
    if (!hPsRes) {
        MessageBox(nullptr, L"Pixel shader resource not found", L"Error", MB_OK);
        vsBlob->Release();
        return false;
    }

    HGLOBAL hPsData = LoadResource(nullptr, hPsRes);
    if (!hPsData) {
        MessageBox(nullptr, L"Failed to load pixel shader resource", L"Error", MB_OK);
        vsBlob->Release();
        return false;
    }

    DWORD psSize = SizeofResource(nullptr, hPsRes);
    LPVOID psData = LockResource(hPsData);

    ID3DBlob* psBlob = nullptr;
    errorBlob = nullptr;

    hr = D3DCompile(
        psData,
        psSize,
        nullptr,
        nullptr,
        nullptr,
        "main",
        "ps_5_0",
        compileFlags,
        0,
        &psBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA("Pixel shader compilation error:\n");
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        vsBlob->Release();
        UnlockResource(hPsData);
        FreeResource(hPsData);
        return false;
    }

    hr = device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        &pixelShader
    );

    psBlob->Release();
    UnlockResource(hPsData);
    FreeResource(hPsData);

    if (FAILED(hr)) {
        vsBlob->Release();
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = ARRAYSIZE(layout);

    hr = device->CreateInputLayout(
        layout,
        numElements,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &inputLayout
    );

    vsBlob->Release();
    UnlockResource(hVsData);
    FreeResource(hVsData);

    if (FAILED(hr)) {
        return false;
    }

    return true;
}

void Shader::Bind(ID3D11DeviceContext* context) {
    context->IASetInputLayout(inputLayout);
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);
}