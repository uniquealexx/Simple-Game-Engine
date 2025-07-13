cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

struct VS_IN
{
    float3 pos : POSITION;
    float3 color : COLOR;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

PS_IN main(VS_IN input)
{
    PS_IN output;
    float4 pos = float4(input.pos, 1.0f);

    pos = mul(pos, world);
    pos = mul(pos, view);
    pos = mul(pos, projection);
    
    output.pos = pos;
    output.color = input.color;
    return output;
}