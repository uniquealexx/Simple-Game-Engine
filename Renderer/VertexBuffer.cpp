#include "VertexBuffer.hpp"

VertexBuffer::VertexBuffer() {}

VertexBuffer::~VertexBuffer() {
    if (buffer) buffer->Release();
}

bool VertexBuffer::Create(ID3D11Device* device, const void* data, UINT stride, UINT count) {
    this->stride = stride;
    this->vertexCount = count;

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = stride * count;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;

    HRESULT hr = device->CreateBuffer(&desc, &initData, &buffer);
    return SUCCEEDED(hr);
}

void VertexBuffer::Bind(ID3D11DeviceContext* context) const {
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
}