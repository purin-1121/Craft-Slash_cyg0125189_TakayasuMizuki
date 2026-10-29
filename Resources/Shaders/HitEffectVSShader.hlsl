cbuffer TransformBuffer : register(b0)
{
    row_major float4x4 worldViewProj;
};

struct BoneTransform
{
    row_major float4x4 transform;
};

StructuredBuffer<BoneTransform> boneTransforms : register(t1);

struct VSInput
{
    float3 position : SV_Position;
    float2 uv : TEXCOORD0;
    uint4 boneIDs : BLENDINDICES0;
    float4 weights : BLENDWEIGHT0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    // 頂点の元の位置を用意
    float4 bindPosition = float4(input.position, 1.0f);
    // 影響を受けるボーンの変換を合成
    float4 skinnedPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float totalWeight = 0.0f;

    [unroll]
    for (uint i = 0; i < 4; ++i)
    {
        // ボーンの影響力
        float weight = input.weights[i];
        if (weight > 0.0f)
        {
            // 元の位置を行列で変換し、影響力を掛ける
            skinnedPosition += mul(
                bindPosition,
                boneTransforms[input.boneIDs[i]].transform) * weight;
            totalWeight += weight;
        }
    }

    // 重みの合計を正規化
    if (totalWeight > 0.0f)
        skinnedPosition /= totalWeight;
    else
        skinnedPosition = bindPosition;

    // 描画用の座標に変換する
    output.position = mul(skinnedPosition, worldViewProj);
    output.uv = input.uv;
    return output;
}