cbuffer ShadowTransformBuffer : register(b1)
{
    row_major float4x4 worldViewProj; // 位置行列
    float alpha; // 地面との距離
    float padding[3]; // パディング
};

struct VS_INPUT
{
    float3 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};