#pragma once

#include "Include.hpp"

struct Vertex {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 color;
};

class Primitive2D {
public:
    Primitive2D() = default;
    virtual ~Primitive2D() = default;

    virtual const Vertex* GetVertices() const = 0;
    virtual UINT GetVertexCount() const = 0;
    virtual D3D11_PRIMITIVE_TOPOLOGY GetTopology() const = 0;
    virtual bool Is3D() const { return false; }
};

class Triangle : public Primitive2D {
public:
    Triangle() {
        vertices[0] = { { 0.0f,  0.5f, 0.0f }, {1, 0, 0} };
        vertices[1] = { { 0.5f, -0.5f, 0.0f }, {0, 1, 0} };
        vertices[2] = { {-0.5f, -0.5f, 0.0f }, {0, 0, 1} };
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
        vertices[0] = { { 100.0f,  100.0f, 0.0f }, {0, 0, 1} };
        vertices[1] = { { 100.0f,  300.0f, 0.0f }, {0, 0, 1} };
        vertices[2] = { { 300.0f,  100.0f, 0.0f }, {0, 0, 1} };
        vertices[3] = { { 300.0f,  300.0f, 0.0f }, {0, 0, 1} };
    }

    const Vertex* GetVertices() const override { return vertices; }
    UINT GetVertexCount() const override { return 4; }
    D3D11_PRIMITIVE_TOPOLOGY GetTopology() const override {
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    }

private:
    Vertex vertices[4];
};

class Cube : public Primitive2D {
public:
    Cube() {
        vertices = {
            // Front face
            {{-0.5f, -0.5f, -0.5f}, {1, 0, 0}},
            {{-0.5f,  0.5f, -0.5f}, {0, 1, 0}},
            {{ 0.5f,  0.5f, -0.5f}, {0, 0, 1}},
            {{ 0.5f, -0.5f, -0.5f}, {1, 1, 0}},

            // Back face
            {{-0.5f, -0.5f, 0.5f}, {1, 0, 1}},
            {{-0.5f,  0.5f, 0.5f}, {0, 1, 1}},
            {{ 0.5f,  0.5f, 0.5f}, {0, 0, 0}},
            {{ 0.5f, -0.5f, 0.5f}, {1, 1, 1}}
        };

        indices = {
            // Front
            0, 1, 2, 0, 2, 3,
            // Back
            4, 6, 5, 4, 7, 6,
            // Top
            1, 5, 6, 1, 6, 2,
            // Bottom
            0, 3, 7, 0, 7, 4,
            // Left
            0, 4, 5, 0, 5, 1,
            // Right
            3, 2, 6, 3, 6, 7
        };
    }

    const Vertex* GetVertices() const override { return vertices.data(); }
    UINT GetVertexCount() const override { return static_cast<UINT>(vertices.size()); }
    const UINT* GetIndices() const { return indices.data(); }
    UINT GetIndexCount() const { return static_cast<UINT>(indices.size()); }
    D3D11_PRIMITIVE_TOPOLOGY GetTopology() const override {
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
    bool Is3D() const override { return true; } 

private:
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;
};