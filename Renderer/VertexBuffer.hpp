#pragma once

#include "Include.hpp"

class VertexBuffer {
public:
    VertexBuffer();
    ~VertexBuffer();

    bool Create(ID3D11Device* device, const void* data, UINT stride, UINT count);
    void Bind(ID3D11DeviceContext* context) const;
    UINT GetVertexCount() const { return vertexCount; }

private:
    ID3D11Buffer* buffer = nullptr;
    UINT stride = 0;
    UINT vertexCount = 0;
};