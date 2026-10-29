Texture2D diffuseTexture : register(t0);
SamplerState textureSampler : register(s0);

cbuffer HitEffectConstants : register(b0)
{
    float whiteAmount;
    float time;
    float2 padding;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET0
{
    float4 color = diffuseTexture.Sample(textureSampler, input.uv);

    float pulse = 0.5f + 0.5f * sin(time * 31.4f);
    float amount = saturate(whiteAmount) * pulse;
    
    // whiteAmount が 0 なら元の色、1 なら白
    // sin波で点滅させる
    color.rgb = lerp(color.rgb, float3(1.0f, 1.0f, 1.0f),amount);

    return color; // 元のアルファ値は維持
}