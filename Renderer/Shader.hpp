#pragma once

#include "Include.hpp"

class Shader {
public:
    Shader();
    ~Shader();

    bool Load(ID3D11Device* device, 
        int vsResourceId, 
        int psResourceId);

    void Bind(ID3D11DeviceContext* context);

    ID3D11InputLayout* GetInputLayout() const { return inputLayout; }

private:
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;
};