#pragma once

#include "Include.hpp"

class VertexBuffer {
public:
    VertexBuffer();
    ~VertexBuffer();

    bool Create(ID3D11Device* device, const void* data, UINT stride, UINT count);
    void Bind(ID3D11DeviceContext* context) const;
    UINT GetVertexCount() const { return vertexCount; }

    bool CreateIndexBuffer(ID3D11Device* device, const UINT* data, UINT count);
    void BindIndexBuffer(ID3D11DeviceContext* context) const;
    UINT GetIndexCount() const { return indexCount; }

private:
    ID3D11Buffer* buffer = nullptr;
    UINT stride = 0;
    UINT vertexCount = 0;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    UINT indexCount = 0;
};