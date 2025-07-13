#pragma once

#include "Include.hpp"

struct Vertex {
    DirectX::XMFLOAT2 position;
    DirectX::XMFLOAT3 color;
};

class Primitive2D {
public:
    Primitive2D() = default;
    virtual ~Primitive2D() = default;

    virtual const Vertex* GetVertices() const = 0;
    virtual UINT GetVertexCount() const = 0;
    virtual D3D11_PRIMITIVE_TOPOLOGY GetTopology() const = 0;
};

class Triangle : public Primitive2D {
public:
    Triangle() {
        vertices[0] = { { 0.0f,  0.5f}, {1, 0, 0} };
        vertices[1] = { { 0.5f, -0.5f}, {0, 1, 0} };
        vertices[2] = { {-0.5f, -0.5f}, {0, 0, 1} };
    }

    const Vertex* GetVertices() const override { return vertices; }
    UINT GetVertexCount() const override { return 3; }
    D3D11_PRIMITIVE_TOPOLOGY GetTopology() const override {
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }

private:
    Vertex vertices[3];
};

class Quad : public Primitive2D {
public:
    Quad() {
        vertices[0] = { {-0.8f,  0.8f}, {0, 0, 1} };
        vertices[1] = { {-0.8f,  0.3f}, {0, 0, 1} };
        vertices[2] = { {-0.3f,  0.8f}, {0, 0, 1} };
        vertices[3] = { {-0.3f,  0.3f}, {0, 0, 1} };
    }

    const Vertex* GetVertices() const override { return vertices; }
    UINT GetVertexCount() const override { return 4; }
    D3D11_PRIMITIVE_TOPOLOGY GetTopology() const override {
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    }

private:
    Vertex vertices[4];
};